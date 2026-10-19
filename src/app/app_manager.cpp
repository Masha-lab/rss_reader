#include "app_manager.h"
#include <curl/curl.h>
#include <iostream>
#include <chrono>
#include "../core/feed_parser_factory.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

namespace app {

void AppManager::addSubscription(const std::string& url) {
    std::lock_guard<std::mutex> lock(mtx_);
    FeedState new_state;
    new_state.feed.url = url;
    feeds_.push_back(new_state);
}

std::string AppManager::downloadUrl(const std::string& url) {
    std::cerr << "[DEBUG] Скачиваю: " << url << "\n";
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("curl_easy_init failed");
    }
    
    std::string readBuffer;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; RSSReader/1.0)");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        std::string err = curl_easy_strerror(res);
        std::cerr << "[DEBUG] Ошибка curl: " << err << "\n";
        throw std::runtime_error(err);
    }
    
    std::cerr << "[DEBUG] Скачано байт: " << readBuffer.size() << "\n";
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
        std::cerr << "[DEBUG] Загружаю ленту " << index << "\n";
        
        std::string xml = downloadUrl(feeds_[index].feed.url);
        
        auto parser = core::FeedParserFactory::create(xml);
        if (!parser) {
            throw std::runtime_error("Unknown feed format");
        }
        
        feeds_[index].feed = parser->parse(xml);
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

} 