#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include "app/app_manager.h"
#include "tui/tui_app.h"

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
    
    // Загружаем ленты вручную (ждем завершения)
    std::cout << "Загружаем ленты...\n";
    for (size_t i = 0; i < manager.getFeeds().size(); ++i) {
        manager.loadFeed(i);
    }
    
    // Сохраняем состояние
    manager.saveState("state.json");
    
    std::cout << "Запуск TUI...\n";
    tui::TuiApp app(manager);
    app.run();
    
    return 0;
}
