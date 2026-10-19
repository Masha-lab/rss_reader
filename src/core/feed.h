#pragma once

#include <string>
#include <vector>

namespace core {

struct Item {
    std::string title;
    std::string link;
    std::string description;
    std::string guid;
    std::string pub_date;
};

struct Feed {
    std::string title;
    std::string link;
    std::string description;
    std::string url;
    std::string etag;
    std::string last_modified;
    std::vector<Item> items;
};

} // namespace core
