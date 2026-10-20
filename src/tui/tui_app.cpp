#include "tui_app.h"
#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace ftxui;

namespace tui {

TuiApp::TuiApp(app::AppManager& manager) : manager_(manager) {
    key_config_.load("keybindings.json");
}

void TuiApp::update_article_cache() {
    auto feeds = manager_.getFeeds();
    if (selected_feed_idx_ >= feeds.size() || 
        selected_article_idx_ >= feeds[selected_feed_idx_].feed.items.size()) {
        cached_article_text_ = "Статья не найдена";
        cached_article_links_.clear();
        cache_valid_ = true;
        return;
    }
    
    auto& item = feeds[selected_feed_idx_].feed.items[selected_article_idx_];
    
    try {
        auto result = converter_.convert(item.description);
        cached_article_text_ = result.text;
        cached_article_links_ = result.links;
    } catch (const std::exception& e) {
        cached_article_text_ = "Ошибка конвертации: " + std::string(e.what());
        cached_article_links_.clear();
    }
    
    cache_valid_ = true;
}

Element TuiApp::draw_feeds_screen() {
    auto feeds = manager_.getFeeds();
    Elements list;
    for (size_t i = 0; i < feeds.size(); ++i) {
        std::string status_icon = "[  ] ";
        if (feeds[i].status == app::LoadStatus::OK) status_icon = "[OK] ";
        else if (feeds[i].status == app::LoadStatus::LOADING) status_icon = "[..] ";
        else if (feeds[i].status == app::LoadStatus::ERROR) status_icon = "[!!] ";
        
        std::string title = feeds[i].feed.title.empty() ? feeds[i].feed.url : feeds[i].feed.title;
        auto item = hbox({text(status_icon), text(title), filler(), text(" (" + std::to_string(feeds[i].feed.items.size()) + ")") | dim});
        if (i == selected_feed_idx_) item = item | inverted;
        list.push_back(item);
    }
    if (list.empty()) list.push_back(text("Нет подписок") | dim);
    
    return vbox({
        text("RSS Reader - Ленты") | bold | center, separator(), 
        vbox(list) | vscroll_indicator | yframe | size(HEIGHT, EQUAL, 15), filler(), separator(), 
        text("j/k - навигация | Enter - открыть | x - выход") | dim | center
    });
}

Element TuiApp::draw_articles_screen() {
    auto feeds = manager_.getFeeds();
    if (selected_feed_idx_ >= feeds.size()) return text("Лента не выбрана") | center;
    auto& feed = feeds[selected_feed_idx_];
    Elements list;
    for (size_t i = 0; i < feed.feed.items.size(); ++i) {
        auto item = text(feed.feed.items[i].title);
        if (i == selected_article_idx_) item = item | inverted;
        list.push_back(item);
    }
    if (list.empty()) list.push_back(text("Нет статей") | dim);
    
    return vbox({
        text("Лента: " + feed.feed.title) | bold | center, separator(), 
        vbox(list) | vscroll_indicator | yframe | size(HEIGHT, EQUAL, 18), filler(), separator(), 
        text("j/k - навигация | Enter - читать | h - назад | x - выход") | dim | center
    });
}

Element TuiApp::draw_article_screen() {
    if (!cache_valid_) {
        update_article_cache();
    }
    
    Elements content;
    std::istringstream stream(cached_article_text_);
    std::string line;
    int line_count = 0;
    
    while (std::getline(stream, line) && line_count < 30) {
        content.push_back(text(line));
        line_count++;
    }
    
    if (line_count == 30) {
        content.push_back(text("... (текст обрезан) ...") | dim);
    }
    
    return vbox({
        text("Просмотр статьи") | bold | center,
        separator(),
        vbox(content),
        filler(),
        separator(),
        text("=== x - ВЫХОД | h - НАЗАД | 1-5 - ссылки ===") | bold | color(Color::Yellow) | center
    });
}

void TuiApp::run() {
    auto screen = ScreenInteractive::Fullscreen();
    auto observer = std::make_shared<TuiLoadObserver>(screen);
    manager_.addObserver(observer);
    
    // ВАЖНО: используем QuitClosure для корректного выхода
    auto quit_closure = screen.ExitLoopClosure();
    
    // Создаём Renderer
    auto renderer = Renderer([&] {
        switch (current_screen_) {
            case Screen::FEEDS: return draw_feeds_screen();
            case Screen::ARTICLES: return draw_articles_screen();
            case Screen::ARTICLE: return draw_article_screen();
        }
        return text("Unknown screen");
    });
    
    // Оборачиваем в CatchEvent с правильным возвратом значений
    auto component = CatchEvent(renderer, [this, &quit_closure](Event event) {
        // ВЫХОД
        if (event == Event::Character('x') || event == Event::Character('X') || 
            event == Event::Character('q') || event == Event::Character('Q') ||
            event == Event::Escape) {
            quit_closure();  // Корректный выход из Loop
            return true;
        }
        
        // НАЗАД
        if (event == Event::Character('h') || event == Event::Backspace) {
            if (current_screen_ == Screen::ARTICLE) {
                current_screen_ = Screen::ARTICLES;
                cache_valid_ = false;
            }
            else if (current_screen_ == Screen::ARTICLES) {
                current_screen_ = Screen::FEEDS;
            }
            return true;
        }

        // ВПЕРЁД
        if (event == Event::Return) {
            if (current_screen_ == Screen::FEEDS) { 
                current_screen_ = Screen::ARTICLES; 
                selected_article_idx_ = 0; 
            }
            else if (current_screen_ == Screen::ARTICLES) { 
                current_screen_ = Screen::ARTICLE;
                cache_valid_ = false;
            }
            return true;
        }

        // ВНИЗ
        if (event == Event::Character('j') || event == Event::ArrowDown) {
            if (current_screen_ == Screen::FEEDS && selected_feed_idx_ < manager_.getFeeds().size() - 1) selected_feed_idx_++;
            else if (current_screen_ == Screen::ARTICLES) {
                auto f = manager_.getFeeds();
                if (selected_feed_idx_ < f.size() && selected_article_idx_ < f[selected_feed_idx_].feed.items.size() - 1) selected_article_idx_++;
            }
            return true;
        }

        // ВВЕРХ
        if (event == Event::Character('k') || event == Event::ArrowUp) {
            if (current_screen_ == Screen::FEEDS && selected_feed_idx_ > 0) selected_feed_idx_--;
            else if (current_screen_ == Screen::ARTICLES && selected_article_idx_ > 0) selected_article_idx_--;
            return true;
        }

        // ЦИФРЫ 1-5 для открытия ссылок (Требование №34)
        if (current_screen_ == Screen::ARTICLE && event.is_character()) {
            std::string ch = event.character();
            if (ch.length() == 1 && ch[0] >= '1' && ch[0] <= '5') {
                int link_idx = ch[0] - '1';
                auto feeds = manager_.getFeeds();
                if (selected_feed_idx_ < feeds.size() && selected_article_idx_ < feeds[selected_feed_idx_].feed.items.size()) {
                    auto& item = feeds[selected_feed_idx_].feed.items[selected_article_idx_];
                    auto result = converter_.convert(item.description);
                    if (link_idx < (int)result.links.size()) {
                        std::string url = result.links[link_idx];
                        std::string cmd = "xdg-open \"" + url + "\" &";
                        system(cmd.c_str());
                    }
                }
                return true;
            }
        }

        // ВАЖНО: возвращаем true для всех остальных событий, чтобы Loop не завершался
        return true;
    });
    
    screen.Loop(component);
}

} // namespace tui
