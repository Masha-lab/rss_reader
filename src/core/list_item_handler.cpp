#include "list_item_handler.h"

namespace core {

std::string ListItemHandler::handleOpenTag(const std::string& tag_name) {
    (void)tag_name;
    return "\n- ";  
}

std::string ListItemHandler::handleCloseTag(const std::string& tag_name) {
    (void)tag_name;
    return "";  
}

} 