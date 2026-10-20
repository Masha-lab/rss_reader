#include "key_config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace app {

void KeyConfig::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[DEBUG] Файл клавиш не найден, используем значения по умолчанию\n";
        return;
    }
    
    try {
        json j;
        file >> j;
        
        for (auto& [action, keys] : j.items()) {
            if (keys.is_array()) {
                bindings_[action] = keys.get<std::vector<std::string>>();
            } else {
                bindings_[action] = {keys.get<std::string>()};
            }
        }
        
        std::cerr << "[DEBUG] Загружено назначений клавиш: " << bindings_.size() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[DEBUG] Ошибка чтения конфига клавиш: " << e.what() << "\n";
    }
}

bool KeyConfig::matches(const std::string& action, const std::string& key) const {
    auto it = bindings_.find(action);
    if (it == bindings_.end()) return false;
    
    for (const auto& bound_key : it->second) {
        if (bound_key == key) return true;
    }
    return false;
}

} // namespace app
