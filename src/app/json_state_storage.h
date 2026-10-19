#pragma once

#include "state_storage.h"
#include <string>

namespace app {

class JsonStateStorage : public IStateStorage {
public:
    explicit JsonStateStorage(const std::string& filepath);
    void save(const std::vector<FeedState>& feeds) override;
    std::vector<FeedState> load() override;

private:
    std::string filepath_;
};

} // namespace app
