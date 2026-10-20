#include "tui_app.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

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

void TuiApp::perform_search() {
    search_results_.clear();
    if (search_query_.empty()) return;
    
    auto feeds = manager_.getFeeds();
    if (selected_feed_idx_ >= feeds.size()) return;
    
    auto& feed = feeds[selected_feed_idx_];
    std::string query_lower = search_query_;
    std::transform(query_lower.begin(), query_lower.end(), query_lower.begin(), ::tolower);
    
    for (size_t i = 0; i < feed.feed.items.size(); ++i) {
        std::string title_lower = feed.feed.items[i].title;
        std::transform(title_lower.begin(), title_lower.end(), title_lower.begin(), ::tolower);
        
        if (title_lower.find(query_lower) != std::string::npos) {
            search_results_.push_back(i);
        }
    }
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
        text("j/k - навигация | Enter - открыть | / - поиск | Ctrl+C - выход") | dim | center
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
        text("j/k - навигация | Enter - читать | h - назад | / - поиск | Ctrl+C - выход") | dim | center
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
        text("=== Ctrl+C - ВЫХОД | h - НАЗАД | 1-5 - ссылки ===") | bold | color(Color::Yellow) | center
    });
}

Element TuiApp::draw_search_screen() {
    auto feeds = manager_.getFeeds();
    if (selected_feed_idx_ >= feeds.size()) return text("Лента не выбрана") | center;
    
    Elements results;
    for (size_t i = 0; i < search_results_.size(); ++i) {
        size_t idx = search_results_[i];
        auto item = text(feeds[selected_feed_idx_].feed.items[idx].title);
        if (i == selected_search_idx_) item = item | inverted;
        results.push_back(item);
    }
    
    if (results.empty()) {
        results.push_back(text("Ничего не найдено по запросу \"" + search_query_ + "\"") | dim);
    }
    
    return vbox({
        text("Поиск в ленте") | bold | center,
        separator(),
        text("Запрос: " + search_query_) | bold | color(Color::Cyan),
        separator(),
        vbox(results) | vscroll_indicator | yframe | size(HEIGHT, EQUAL, 15),
        filler(),
        separator(),
        text("Enter - открыть | h - закрыть поиск | Ctrl+C - выход") | dim | center
    });
}

bool TuiApp::handle_event(Event event) {
    // Обработка экрана поиска
    if (current_screen_ == Screen::SEARCH) {
        // Закрытие поиска
        if (event == Event::Character('h') || event == Event::Backspace) {
            current_screen_ = Screen::ARTICLES;
            return true;
        }
        
        // Ввод символов в поле поиска
        if (event.is_character() && event.character().length() == 1) {
            char c = event.character()[0];
            if (c >= ' ' && c <= '~') {
                search_query_ += c;
                perform_search();
                selected_search_idx_ = 0;
                return true;
            }
            if (c == '\b' || c == 127) {
                if (!search_query_.empty()) {
                    search_query_.pop_back();
                    perform_search();
                    selected_search_idx_ = 0;
                }
                return true;
            }
        }
        
        // Навигация по результатам поиска
        if (event == Event::ArrowDown || event == Event::Character('j')) {
            if (!search_results_.empty() && selected_search_idx_ < search_results_.size() - 1) {
                selected_search_idx_++;
            }
            return true;
        }
        if (event == Event::ArrowUp || event == Event::Character('k')) {
            if (selected_search_idx_ > 0) {
                selected_search_idx_--;
            }
            return true;
        }
        
        // Открытие выбранной статьи из результатов
        if (event == Event::Return && !search_results_.empty()) {
            selected_article_idx_ = search_results_[selected_search_idx_];
            current_screen_ = Screen::ARTICLE;
            cache_valid_ = false;
            return true;
        }
        
        return true;
    }
    
    // Открытие поиска (клавиша '/')
    if (event == Event::Character('/') && current_screen_ != Screen::SEARCH) {
        current_screen_ = Screen::SEARCH;
        search_query_ = "";
        search_results_.clear();
        selected_search_idx_ = 0;
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

    // ЦИФРЫ 1-5 для открытия ссылок
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

    return true;
}

void TuiApp::run() {
    auto screen = ScreenInteractive::Fullscreen();
    auto observer = std::make_shared<TuiLoadObserver>(screen);
    manager_.addObserver(observer);
    
    auto renderer = Renderer([&] {
        switch (current_screen_) {
            case Screen::FEEDS: return draw_feeds_screen();
            case Screen::ARTICLES: return draw_articles_screen();
            case Screen::ARTICLE: return draw_article_screen();
            case Screen::SEARCH: return draw_search_screen();
        }
        return text("Unknown screen");
    });
    
    auto component = CatchEvent(renderer, [this](Event event) {
        return handle_event(event);
    });
    
    screen.Loop(component);
}

} // namespace tui
