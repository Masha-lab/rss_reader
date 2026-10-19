#pragma once

#include "html_to_text.h"
#include <vector>
#include <string>

namespace core {

class LinkHandler : public HtmlTagHandler {
public:
    
    explicit LinkHandler(std::vector<std::string>& links);
    
    std::string handleOpenTag(const std::string& tag_name) override;
    std::string handleCloseTag(const std::string& tag_name) override;

private:
    std::vector<std::string>& links_;  // Ссылка на общий список ссылок
};

}