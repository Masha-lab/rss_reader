#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace app {

class KeyConfig {
public:
    void load(const std::string& filepath);
    bool matches(const std::string& action, const std::string& key) const;
    
private:
    std::unordered_map<std::string, std::vector<std::string>> bindings_;
};

} // namespace app
