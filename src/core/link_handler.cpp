#include "link_handler.h"

namespace core {

LinkHandler::LinkHandler(std::vector<std::string>& links) 
    : links_(links) {}

std::string LinkHandler::handleOpenTag(const std::string& tag_name) {
    (void)tag_name;
    size_t link_number = links_.size();
    return "[" + std::to_string(link_number) + "] ";
}

std::string LinkHandler::handleCloseTag(const std::string& tag_name) {
    (void)tag_name;
    return "";
}

} // namespace core
