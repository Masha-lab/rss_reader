#pragma once

#include <string>
#include <vector>
#include "../core/feed.h"

namespace app {

enum class LoadStatus { IDLE, LOADING, OK, ERROR };

struct FeedState {
    core::Feed feed;
    LoadStatus status = LoadStatus::IDLE;
    std::string error_message;
    std::vector<std::string> read_guids;
};

} // namespace app
