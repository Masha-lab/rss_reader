#include <iostream>
#include "app_manager.h"

int main() {
    
    app::AppManager manager;
    manager.addSubscription("https://habr.com/rss/");
    std::cout << "Start..\n";
    manager.loadFeed(0);

    auto feeds = manager.getFeeds();
    std::cout << "Статус: " << static_cast<int>(feeds[0].status) << "\n";
    std::cout << "Название ленты: " << feeds[0].feed.title << "\n";
    std::cout << "Статей: " << feeds[0].feed.items.size() << "\n";

    return 0;
}
