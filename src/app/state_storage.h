#pragma once

#include <string>
#include <vector>
#include <memory>
#include "feed_state.h"

namespace app {

// Интерфейс для сохранения/загрузки состояния (паттерн Strategy)
class IStateStorage {
public:
    virtual ~IStateStorage() = default;
    virtual void save(const std::vector<FeedState>& feeds) = 0;
    virtual std::vector<FeedState> load() = 0;
};

} // namespace app
