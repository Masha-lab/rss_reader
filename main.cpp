#include <iostream>
#include <cstdlib>
#include "app/app_manager.h"
#include "tui/tui_app.h"

int main() {
    setenv("LSAN_OPTIONS", "detect_leaks=0", 1);
    
    app::AppManager manager;
    manager.loadState("state.json");
    
    auto feeds = manager.getFeeds();
    if (feeds.empty()) {
        std::cout << "Добавляем тестовую ленту...\n";
        manager.addSubscription("https://rss.nytimes.com/services/xml/rss/nyt/HomePage.xml");
    }
    
    std::cout << "Загружаем ленты...\n";
    for (size_t i = 0; i < manager.getFeeds().size(); ++i) {
        manager.loadFeed(i);
    }
    
    manager.saveState("state.json");
    
    std::cout << "Запуск TUI...\n";
    
    tui::TuiApp app(manager);
    app.run();
    
    std::cout << "TUI завершен.\n";
    return 0;
}
