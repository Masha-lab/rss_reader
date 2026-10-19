#include "html_to_text_converter.h"
#include "paragraph_handler.h"
#include "heading_handler.h"
#include "list_handler.h"
#include "list_item_handler.h"
#include "link_handler.h"
#include <iostream>

namespace core {

void HtmlToTextConverter::traverseNode(pugi::xml_node node, 
                                        std::string& result, 
                                        std::vector<std::string>& links) {
    if (node.type() == pugi::node_pcdata) {
        result += node.text().as_string();
    } 
    else if (node.type() == pugi::node_element) {
        std::string tag_name = node.name();
        
        // Для ссылок: сначала добавляем URL в вектор, ПОТОМ создаём обработчик
        if (tag_name == "a") {
            std::string href = node.attribute("href").as_string();
            if (!href.empty()) {
                links.push_back(href);
            }
        }
        
        auto handler = getHandler(tag_name, links);
        
        if (handler) {
            result += handler->handleOpenTag(tag_name);
        }
        
        for (pugi::xml_node child : node.children()) {
            traverseNode(child, result, links);
        }
        
        if (handler) {
            result += handler->handleCloseTag(tag_name);
        }
    }
}

std::unique_ptr<HtmlTagHandler> HtmlToTextConverter::getHandler(
    const std::string& tag_name, 
    std::vector<std::string>& links) {
    
    if (tag_name == "p") {
        return std::make_unique<ParagraphHandler>();
    }
    else if (tag_name == "h1" || tag_name == "h2" || tag_name == "h3") {
        return std::make_unique<HeadingHandler>();
    }
    else if (tag_name == "ul" || tag_name == "ol") {
        return std::make_unique<ListHandler>();
    }
    else if (tag_name == "li") {
        return std::make_unique<ListItemHandler>();
    }
    else if (tag_name == "a") {
        return std::make_unique<LinkHandler>(links);
    }
    
    return nullptr;
}

HtmlToTextResult HtmlToTextConverter::convert(const std::string& html) {
    HtmlToTextResult result;
    
    std::string wrapped_html = "<root>" + html + "</root>";
    
    pugi::xml_document doc;
    pugi::xml_parse_result parse_result = doc.load_string(wrapped_html.c_str());
    
    if (!parse_result) {
        std::cerr << "[DEBUG] Ошибка парсинга HTML: " << parse_result.description() << "\n";
        return result;
    }
    
    traverseNode(doc.document_element(), result.text, result.links);
    
    if (!result.links.empty()) {
        result.text += "\n\n---\n";
        for (size_t i = 0; i < result.links.size(); ++i) {
            result.text += "[" + std::to_string(i + 1) + "] " + result.links[i] + "\n";
        }
    }
    
    return result;
}

} // namespace core
