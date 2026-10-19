#pragma once

#include <string>
#include <vector>
#include <memory>
#include <pugixml.hpp>
#include "html_to_text.h"

namespace core{

    struct HtmlToTextResult{
        std::string text;
        std::vector<std::string> links;
    };

    class HtmlToTextConverter{
        public:
        HtmlToTextResult convert(const std::string& html);
        private:
        void traverseNode(pugi::xml_node node, std::string& result, std::vector<std::string>& links);
        std::unique_ptr<HtmlTagHandler> getHandler(const std::string& tag_name, std::vector<std::string>& links);
    };


}