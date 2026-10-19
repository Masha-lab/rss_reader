#pragma once

#include "html_to_text.h"

namespace core {

class ListItemHandler : public HtmlTagHandler {
public:
    std::string handleOpenTag(const std::string& tag_name) override;
    std::string handleCloseTag(const std::string& tag_name) override;
};

} 