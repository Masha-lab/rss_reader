#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <memory>
#include "../core/feed.h"
#include "feed_state.h"
#include "state_storage.h"

namespace app {

class AppManager {
public:
    void addSubscription(const std::string& url);
    void loadFeed(size_t index);
    std::vector<FeedState> getFeeds() const;
    void startBackgroundLoading();
    void saveState(const std::string& filepath);
    void loadState(const std::string& filepath);

private:
    std::string downloadUrl(const std::string& url, std::string& out_etag, std::string& out_last_modified);

    mutable std::mutex mtx_;
    std::vector<FeedState> feeds_;
    std::unordered_set<std::string> read_guids_;
    std::jthread loader_thread_;
};

} // namespace app
