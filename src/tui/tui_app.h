#pragma once

#include <memory>
#include <vector>
#include <string>
#include "../app/app_manager.h"
#include "../core/html_to_text_converter.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace tui {

class TuiApp {
public:
    explicit TuiApp(app::AppManager& manager);
    void run();

private:
    app::AppManager& manager_;
    core::HtmlToTextConverter converter_;
    
    enum class Screen { FEEDS, ARTICLES, ARTICLE };
    Screen current_screen_ = Screen::FEEDS;
    
    size_t selected_feed_idx_ = 0;
    size_t selected_article_idx_ = 0;
    
    // Методы отрисовки для каждого экрана
    ftxui::Element draw_feeds_screen();
    ftxui::Element draw_articles_screen();
    ftxui::Element draw_article_screen();
    
    // Обработка клавиш
    bool handle_event(ftxui::Event event);
};

} // namespace tui
