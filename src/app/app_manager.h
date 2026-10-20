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
#include "load_observer.h"

namespace app {

class AppManager {
public:
    void addSubscription(const std::string& url);
    void loadFeed(size_t index);
    std::vector<FeedState> getFeeds() const;
    void startBackgroundLoading();
    void saveState(const std::string& filepath);
    void loadState(const std::string& filepath);
    
    // Методы для Observer
    void addObserver(std::shared_ptr<ILoadObserver> observer);
    void removeObserver(std::shared_ptr<ILoadObserver> observer);

private:
    std::string downloadUrl(const std::string& url, std::string& out_etag, std::string& out_last_modified);
    void notifyObservers(size_t feed_index, bool success, const std::string& error_message);

    mutable std::mutex mtx_;
    std::vector<FeedState> feeds_;
    std::unordered_set<std::string> read_guids_;
    std::jthread loader_thread_;
    std::vector<std::weak_ptr<ILoadObserver>> observers_;
};

} // namespace app
