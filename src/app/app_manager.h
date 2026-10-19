#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <mutex>
#include <thread>
#include "../core/feed.h"

namespace app {

enum class LoadStatus { IDLE, LOADING, OK, ERROR };

struct FeedState {
    core::Feed feed;
    LoadStatus status = LoadStatus::IDLE;
    std::string error_message;
};

class AppManager {
public:
    void addSubscription(const std::string& url);
    void loadFeed(size_t index);
    std::vector<FeedState> getFeeds() const;
    void startBackgroundLoading();

private:
    std::string downloadUrl(const std::string& url, std::string& out_etag, std::string& out_last_modified);

    mutable std::mutex mtx_;
    std::vector<FeedState> feeds_;
    std::unordered_set<std::string> read_guids_;
    std::jthread loader_thread_;
};

} // namespace app
