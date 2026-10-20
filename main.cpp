#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "app/app_manager.h"
#include "app/feed_state.h"
#include "core/feed_parser_factory.h"
#include "tui/tui_app.h"

int main() {
    setenv("LSAN_OPTIONS", "detect_leaks=0", 1);
    
    app::AppManager manager;
    manager.loadState("state.json");
    
    auto feeds = manager.getFeeds();
    if (feeds.empty()) {
        std::cout << "Добавляем тестовые ленты...\n";
        manager.addSubscription("https://rss.nytimes.com/services/xml/rss/nyt/HomePage.xml");
        
        std::ifstream file("test_with_links.xml");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string xml = buffer.str();
            
            auto parser = core::FeedParserFactory::create(xml);
            if (parser) {
                app::FeedState test_state;
                test_state.feed = parser->parse(xml);
                test_state.feed.url = "local://test_with_links.xml";
                test_state.status = app::LoadStatus::OK;
                manager.addFeed(test_state);
                std::cout << "Тестовая лента добавлена\n";
            }
        } else {
            std::cout << "Файл test_with_links.xml не найден\n";
        }
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
