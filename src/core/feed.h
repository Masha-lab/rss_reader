#pragma once

#include <string>
#include <vector>

namespace core {

struct Item {
    std::string title;
    std::string link;
    std::string description;
    std::string author;
    std::string pub_date;
    std::string guid;
    bool is_read = false;
};

struct Feed {
    std::string url;
    std::string title;
    std::string description;
    std::vector<Item> items;
    std::string etag;
    std::string last_modified;
};

} // namespace core
