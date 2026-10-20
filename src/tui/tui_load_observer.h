#pragma once

#include "../app/load_observer.h"
#include <ftxui/component/screen_interactive.hpp>

namespace tui {

// Наблюдатель, который перерисовывает экран при завершении загрузки
class TuiLoadObserver : public app::ILoadObserver {
public:
    explicit TuiLoadObserver(ftxui::ScreenInteractive& screen) 
        : screen_(screen) {}
    
    void onLoadComplete(size_t /*feed_index*/, bool /*success*/, const std::string& /*error_message*/) override {
        // Асинхронно просим экран перерисоваться
        screen_.PostEvent(ftxui::Event::Custom);
    }

private:
    ftxui::ScreenInteractive& screen_;
};

} // namespace tui
