#include "app_manager.h"
#include <curl/curl.h>
#include "core/feed_parser_factory.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp){
    userp->append((char*)contents, size*nmemb);
    return size*nmemb;
}

namespace app{

    std::string AppManager::downloadUrl(const std::string& url){
        CURL* curl = curl_easy_init();
        if (!curl){
            throw std::runtime_error("Failed to inithial curl");

        }
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK){
            throw std::runtime_error(curl_easy_strerror(res));
        }
        return readBuffer;
    }

    void AppManager::loadFeed(size_t index){
        if (index >= feeds_.size()){
            return;
        }
        std::lock_guard<std::mutex> lpck(mtx_);

        try{
            feeds_[index].status = LoadStatus::LOADING;
            std::string xml = downloadUrl(feeds_[index].feed.url);
            auto parser = core::FeedParserFactory::create(xml);
            if (!parser){
                throw std::runtime_error("Unknow feed format");
            }
            feeds_[index].feed = parser->parse(xml);
            feeds_[index].status = LoadStatus::OK;
        }catch (const std::exception& e){
            feeds_[index].status = LoadStatus::ERROR;
        }
    }

    void AppManager::addSubscription(const std::string& url){
        std::lock_guard<std::mutex> lock(mtx_);
        FeedState new_state;
        new_state.feed.url = url;
        feeds_.push_back(new_state);
    }

    std::vector<FeedState> AppManager::getFeeds() const{
        std::lock_guard<std::mutex> lock(mtx_);
        return feeds_;
    }
}