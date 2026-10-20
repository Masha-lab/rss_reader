#pragma once

#include <cstddef>
#include <string>

namespace app {

// Интерфейс наблюдателя для уведомления о завершении загрузки ленты (Паттерн Observer)
class ILoadObserver {
public:
    virtual ~ILoadObserver() = default;
    
    // Вызывается после успешной загрузки или ошибки
    virtual void onLoadComplete(size_t feed_index, bool success, const std::string& error_message) = 0;
};

} // namespace app
