#pragma once

#include <vector>
#include <string>
#include "../core/feed.h"
#include <unordered_set>

#include <mutex>
#include <thread>

namespace app{

    enum class LoadStatus {IDLE, LOADING, OK, ERROR};

    struct FeedState{
        core::Feed feed;
        LoadStatus status = LoadStatus::IDLE;
    };

    class AppManager {

        public:

        void startBackgroundLoading();

        void loadFeed(size_t index);
        void addSubscription(const std::string& url);

        std::vector<FeedState> getFeeds() const;

        private:

        mutable std::mutex mtx_;
        std::jthread loader_thread_;

        std::string downloadUrl(const std::string& url);

        std::vector<FeedState> feeds_;
        std::unordered_set<std::string> read_guids_;
    };
}