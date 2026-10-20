#include "app_manager.h"
#include "json_state_storage.h"
#include <curl/curl.h>
#include <iostream>
#include <chrono>
#include "../core/feed_parser_factory.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t HeaderCallback(char* buffer, size_t size, size_t nitems, std::string* userdata) {
    std::string header(buffer, size * nitems);
    *userdata += header;
    return size * nitems;
}

namespace app {

void AppManager::addSubscription(const std::string& url) {
    std::lock_guard<std::mutex> lock(mtx_);
    FeedState new_state;
    new_state.feed.url = url;
    feeds_.push_back(new_state);
}

std::string AppManager::downloadUrl(const std::string& url, std::string& out_etag, std::string& out_last_modified) {
    std::cerr << "[DEBUG] Скачиваю: " << url << "\n";

    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("curl_easy_init failed");
    }

    std::string readBuffer;
    std::string headerBuffer;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headerBuffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; RSSReader/1.0)");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    struct curl_slist* headers = nullptr;
    if (!out_etag.empty()) {
        std::string etag_header = "If-None-Match: " + out_etag;
        headers = curl_slist_append(headers, etag_header.c_str());
    }
    if (!out_last_modified.empty()) {
        std::string lm_header = "If-Modified-Since: " + out_last_modified;
        headers = curl_slist_append(headers, lm_header.c_str());
    }
    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }

    auto findHeaderVal = [&](const std::string& name) -> std::string {
        size_t pos = headerBuffer.find(name + ":");
        if (pos == std::string::npos) return "";
        size_t end = headerBuffer.find("\r\n", pos);
        if (end == std::string::npos) end = headerBuffer.length();
        std::string val = headerBuffer.substr(pos + name.length() + 1, end - pos - name.length() - 1);
        while (!val.empty() && val.front() == ' ') val.erase(val.begin());
        while (!val.empty() && val.back() == ' ') val.pop_back();
        return val;
    };

    out_etag = findHeaderVal("ETag");
    out_last_modified = findHeaderVal("Last-Modified");

    if (http_code == 304) {
        std::cerr << "[DEBUG] Лента не изменилась (304 Not Modified)\n";
        return "";
    }

    std::cerr << "[DEBUG] Скачано байт: " << readBuffer.size() << ", ETag: " << out_etag << "\n";
    return readBuffer;
}

void AppManager::loadFeed(size_t index) {
    std::lock_guard<std::mutex> lock(mtx_);

    if (index >= feeds_.size()) {
        std::cerr << "[DEBUG] Неверный индекс: " << index << "\n";
        return;
    }

    try {
        feeds_[index].status = LoadStatus::LOADING;
        std::cerr << "[DEBUG] Загружаю ленту " << index << ", URL: " << feeds_[index].feed.url << "\n";

        std::string current_etag = feeds_[index].feed.etag;
        std::string current_lm = feeds_[index].feed.last_modified;
        std::string current_url = feeds_[index].feed.url;  // сохраняем URL до парсинга

        std::string xml = downloadUrl(feeds_[index].feed.url, current_etag, current_lm);

        if (xml.empty() && !current_etag.empty()) {
            feeds_[index].feed.etag = current_etag;
            feeds_[index].feed.last_modified = current_lm;
            feeds_[index].status = LoadStatus::OK;
            std::cerr << "[DEBUG] Кэш сработал, статей: " << feeds_[index].feed.items.size() << "\n";
            return;
        }

        auto parser = core::FeedParserFactory::create(xml);
        if (!parser) {
            throw std::runtime_error("Unknown feed format");
        }

        auto parsed_feed = parser->parse(xml);
        parsed_feed.url = current_url;  // восстанавливаем URL после парсинга
        feeds_[index].feed = parsed_feed;
        feeds_[index].feed.etag = current_etag;
        feeds_[index].feed.last_modified = current_lm;
        feeds_[index].status = LoadStatus::OK;
        std::cerr << "[DEBUG] Успех! Статей: " << feeds_[index].feed.items.size() << "\n";

    } catch (const std::exception& e) {
        feeds_[index].status = LoadStatus::ERROR;
        feeds_[index].error_message = e.what();
        std::cerr << "[DEBUG] Ошибка: " << e.what() << "\n";
    }
}

std::vector<FeedState> AppManager::getFeeds() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return feeds_;
}

void AppManager::startBackgroundLoading() {
    loader_thread_ = std::jthread([this](std::stop_token stoken) {
        while (!stoken.stop_requested()) {
            for (size_t i = 0; i < feeds_.size(); ++i) {
                if (feeds_[i].status == LoadStatus::IDLE || feeds_[i].status == LoadStatus::ERROR) {
                    loadFeed(i);
                }
            }
            std::this_thread::sleep_for(std::chrono::minutes(15));
        }
    });
}

void AppManager::saveState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mtx_);
    JsonStateStorage storage(filepath);
    storage.save(feeds_);
}

void AppManager::loadState(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mtx_);
    JsonStateStorage storage(filepath);
    feeds_ = storage.load();
}

} // namespace app
