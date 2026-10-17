#pragma once

#include <string>

namespace core{
    class HtmlTagHandler{
        public:
        virtual ~HtmlTagHandler() = default;
        virtual std::string handleOpenTag(const std::string& tag_name) = 0;
        virtual std::string handleCloseTag(const std::string& tag_name) = 0;
    };
}