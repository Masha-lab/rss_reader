#include "paragraph_handler.h"

namespace core{

    std::string ParagraphHandler::handleOpenTag(const std::string& tag_name){
        (void)tag_name;
        return "\n\n";
    }

    std::string ParagraphHandler::handleCloseTag(const std::string& tag_name){
        (void)tag_name;
        return "\n";
    }
}