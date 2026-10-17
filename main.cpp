#include <iostream>
#include "app_manager.h"

int main() {
    
    app::AppManager manager;
    manager.addSubscription("https://bbc.com/rss");
    auto feeds = manager.getFeeds();
    for(const auto& state : feeds){
        static_cast<int>(state.status);
    }
}
