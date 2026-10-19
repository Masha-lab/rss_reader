#include <iostream>
#include <cstdlib>
#include "app/app_manager.h"
#include "core/html_to_text_converter.h"
#include "core/feed_parser_factory.h"

int main() {
    setenv("LSAN_OPTIONS", "detect_leaks=0", 1);
    
    app::AppManager manager;
    
    // Загружаем предыдущее состояние
    manager.loadState("state.json");
    
    // Если лент нет — добавляем тестовую
    auto feeds = manager.getFeeds();
    if (feeds.empty()) {
        std::cout << "Добавляем тестовую ленту...\n";
        manager.addSubscription("https://rss.nytimes.com/services/xml/rss/nyt/HomePage.xml");
    }
    
    std::cout << "Загружаем ленты...\n";
    for (size_t i = 0; i < manager.getFeeds().size(); ++i) {
        manager.loadFeed(i);
    }
    
    // Сохраняем состояние
    manager.saveState("state.json");
    
    // Выводим результат
    feeds = manager.getFeeds();
    for (const auto& state : feeds) {
        std::cout << "\nURL: " << state.feed.url << "\n";
        std::cout << "Статус: " << static_cast<int>(state.status) << "\n";
        std::cout << "Название: " << state.feed.title << "\n";
        std::cout << "Статей: " << state.feed.items.size() << "\n";
        std::cout << "ETag: " << state.feed.etag << "\n";
    }
    
    return 0;
}
