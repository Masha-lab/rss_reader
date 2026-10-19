#include "json_state_storage.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace app {

JsonStateStorage::JsonStateStorage(const std::string& filepath)
    : filepath_(filepath) {}

void JsonStateStorage::save(const std::vector<FeedState>& feeds) {
    json j;
    j["feeds"] = json::array();
    
    for (const auto& state : feeds) {
        json feed_json;
        feed_json["url"] = state.feed.url;
        feed_json["etag"] = state.feed.etag;
        feed_json["last_modified"] = state.feed.last_modified;
        feed_json["title"] = state.feed.title;
        feed_json["read_guids"] = state.read_guids;
        j["feeds"].push_back(feed_json);
    }
    
    std::ofstream file(filepath_);
    if (file.is_open()) {
        file << j.dump(2);
        std::cerr << "[DEBUG] Состояние сохранено в " << filepath_ << "\n";
    } else {
        std::cerr << "[DEBUG] Ошибка: не удалось открыть " << filepath_ << " для записи\n";
    }
}

std::vector<FeedState> JsonStateStorage::load() {
    std::vector<FeedState> feeds;
    
    std::ifstream file(filepath_);
    if (!file.is_open()) {
        std::cerr << "[DEBUG] Файл состояния не найден, начинаем с нуля\n";
        return feeds;
    }
    
    try {
        json j;
        file >> j;
        
        if (j.contains("feeds")) {
            for (const auto& feed_json : j["feeds"]) {
                FeedState state;
                state.feed.url = feed_json.value("url", "");
                state.feed.etag = feed_json.value("etag", "");
                state.feed.last_modified = feed_json.value("last_modified", "");
                state.feed.title = feed_json.value("title", "");
                state.status = LoadStatus::IDLE;
                
                if (feed_json.contains("read_guids")) {
                    state.read_guids = feed_json["read_guids"].get<std::vector<std::string>>();
                }
                
                feeds.push_back(state);
            }
        }
        
        std::cerr << "[DEBUG] Загружено лент: " << feeds.size() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[DEBUG] Ошибка чтения состояния: " << e.what() << "\n";
    }
    
    return feeds;
}

} // namespace app
