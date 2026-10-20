#include "tui_app.h"
#include "tui_load_observer.h"
#include <iostream>
#include <sstream>

using namespace ftxui;

namespace tui {

TuiApp::TuiApp(app::AppManager& manager) : manager_(manager) {}

Element TuiApp::draw_feeds_screen() {
    auto feeds = manager_.getFeeds();
    Elements list;
    
    for (size_t i = 0; i < feeds.size(); ++i) {
        std::string status_icon;
        switch (feeds[i].status) {
            case app::LoadStatus::OK: status_icon = "[OK] "; break;
            case app::LoadStatus::LOADING: status_icon = "[..] "; break;
            case app::LoadStatus::ERROR: status_icon = "[!!] "; break;
            default: status_icon = "[  ] "; break;
        }
        
        std::string title = feeds[i].feed.title.empty() ? feeds[i].feed.url : feeds[i].feed.title;
        std::string unread = std::to_string(feeds[i].feed.items.size());
        
        auto item = hbox({
            text(status_icon),
            text(title),
            filler(),
            text(" (" + unread + ")") | dim
        });
        
        if (i == selected_feed_idx_) {
            item = item | inverted;
        }
        list.push_back(item);
    }
    
    if (list.empty()) {
        list.push_back(text("Нет подписок") | dim);
    }
    
    Elements footer;
    if (!feeds.empty() && feeds[selected_feed_idx_].status == app::LoadStatus::ERROR) {
        footer.push_back(text("Ошибка: " + feeds[selected_feed_idx_].error_message) | color(Color::Red));
        footer.push_back(separator());
    }
    footer.push_back(text("j/k - навигация | Enter - открыть | q - выход") | dim | center);
    
    return vbox({
        text("RSS Reader - Ленты") | bold | center,
        separator(),
        vbox(list) | vscroll_indicator | yframe | size(HEIGHT, EQUAL, 15),
        filler(),
        separator(),
        vbox(footer)
    });
}

Element TuiApp::draw_articles_screen() {
    auto feeds = manager_.getFeeds();
    if (selected_feed_idx_ >= feeds.size()) {
        return text("Лента не выбрана") | center;
    }
    
    auto& feed = feeds[selected_feed_idx_];
    Elements list;
    
    for (size_t i = 0; i < feed.feed.items.size(); ++i) {
        auto item = text(feed.feed.items[i].title);
        if (i == selected_article_idx_) {
            item = item | inverted;
        }
        list.push_back(item);
    }
    
    if (list.empty()) {
        list.push_back(text("Нет статей") | dim);
    }
    
    return vbox({
        text("Лента: " + feed.feed.title) | bold | center,
        separator(),
        vbox(list) | vscroll_indicator | yframe | size(HEIGHT, EQUAL, 18),
        filler(),
        separator(),
        text("j/k - навигация | Enter - читать | h/Backspace - назад | q - выход") | dim | center
    });
}

Element TuiApp::draw_article_screen() {
    auto feeds = manager_.getFeeds();
    if (selected_feed_idx_ >= feeds.size() || 
        selected_article_idx_ >= feeds[selected_feed_idx_].feed.items.size()) {
        return text("Статья не найдена") | center;
    }
    
    auto& item = feeds[selected_feed_idx_].feed.items[selected_article_idx_];
    auto result = converter_.convert(item.description);
    
    Elements content;
    std::istringstream stream(result.text);
    std::string line;
    while (std::getline(stream, line)) {
        content.push_back(text(line));
    }
    
    return vbox({
        text(item.title) | bold | center,
        separator(),
        vbox(content) | vscroll_indicator | yframe | size(HEIGHT, EQUAL, 18),
        filler(),
        separator(),
        text("h/Backspace - назад | q - выход") | dim | center
    });
}

bool TuiApp::handle_event(Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
        return false;
    }
    
    if (event == Event::Character('h') || event == Event::Backspace) {
        if (current_screen_ == Screen::ARTICLE) {
            current_screen_ = Screen::ARTICLES;
        } else if (current_screen_ == Screen::ARTICLES) {
            current_screen_ = Screen::FEEDS;
        }
        return true;
    }
    
    if (event == Event::Return) {
        if (current_screen_ == Screen::FEEDS) {
            current_screen_ = Screen::ARTICLES;
            selected_article_idx_ = 0;
        } else if (current_screen_ == Screen::ARTICLES) {
            current_screen_ = Screen::ARTICLE;
        }
        return true;
    }
    
    if (event == Event::Character('j') || event == Event::ArrowDown) {
        if (current_screen_ == Screen::FEEDS) {
            auto feeds = manager_.getFeeds();
            if (selected_feed_idx_ < feeds.size() - 1) selected_feed_idx_++;
        } else if (current_screen_ == Screen::ARTICLES) {
            auto feeds = manager_.getFeeds();
            if (selected_feed_idx_ < feeds.size()) {
                if (selected_article_idx_ < feeds[selected_feed_idx_].feed.items.size() - 1) {
                    selected_article_idx_++;
                }
            }
        }
        return true;
    }
    
    if (event == Event::Character('k') || event == Event::ArrowUp) {
        if (current_screen_ == Screen::FEEDS && selected_feed_idx_ > 0) {
            selected_feed_idx_--;
        } else if (current_screen_ == Screen::ARTICLES && selected_article_idx_ > 0) {
            selected_article_idx_--;
        }
        return true;
    }
    
    return true;
}

void TuiApp::run() {
    auto screen = ScreenInteractive::Fullscreen();
    
    // Регистрируем наблюдателя для обновления экрана при фоновой загрузке
    auto observer = std::make_shared<TuiLoadObserver>(screen);
    manager_.addObserver(observer);
    
    auto renderer = Renderer([&] {
        switch (current_screen_) {
            case Screen::FEEDS: return draw_feeds_screen();
            case Screen::ARTICLES: return draw_articles_screen();
            case Screen::ARTICLE: return draw_article_screen();
        }
        return text("Unknown screen");
    });
    
    auto component = CatchEvent(renderer, [this](Event event) {
        return handle_event(event);
    });
    
    screen.Loop(component);
}

} // namespace tui
