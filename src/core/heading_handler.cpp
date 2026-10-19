#include "heading_handler.h"

namespace core{

    std::string HeadingHandler::handleOpenTag(const std::string& tag_name){
        (void)tag_name;
        return "\n\n=== ";
    }

    std::string HeadingHandler::handleCloseTag(const std::string& tag_name){
        (void)tag_name;
        return " ===\n";
    }
}