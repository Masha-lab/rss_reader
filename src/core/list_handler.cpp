#include "list_handler.h"

namespace core {

std::string ListHandler::handleOpenTag(const std::string& tag_name) {
    (void)tag_name;
    if (tag_name == "ul") {
        return "\n";  
    }
    return "";
}

std::string ListHandler::handleCloseTag(const std::string& tag_name) {
    (void)tag_name;
    if (tag_name == "ul") {
        return "\n"; 
    }
    return "";
}

}