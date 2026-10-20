#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../app/app_manager.h"
#include "../core/html_to_text_converter.h"
#include "../app/key_config.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "tui_load_observer.h"

namespace tui {

class TuiApp {
public:
    explicit TuiApp(app::AppManager& manager);
    void run();

private:
    app::AppManager& manager_;
    core::HtmlToTextConverter converter_;
    app::KeyConfig key_config_;
    
    enum class Screen { FEEDS, ARTICLES, ARTICLE };
    Screen current_screen_ = Screen::FEEDS;
    
    size_t selected_feed_idx_ = 0;
    size_t selected_article_idx_ = 0;
    
    // Кэш для конвертированного текста статьи
    std::string cached_article_text_;
    std::vector<std::string> cached_article_links_;
    bool cache_valid_ = false;
    
    ftxui::Element draw_feeds_screen();
    ftxui::Element draw_articles_screen();
    ftxui::Element draw_article_screen();
    bool handle_event(ftxui::Event event);
    void update_article_cache();
};

} // namespace tui
