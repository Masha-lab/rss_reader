#include "rss_parser.h"
#include <pugixml.hpp>

namespace core{
    Feed RssParser::parse(const std::string& xml_content){

        Feed feed;
        pugi::xml_document doc;
        doc.load_string(xml_content.c_str());
        pugi::xml_node root = doc.child("rss");
        pugi::xml_node channel = root.child("channel");
        feed.title = channel.child("title").text().as_string();
        
        for(pugi::xml_node item_node : channel.children("item")){
            Item item;
            item.title = item_node.child("title").text().as_string();
            item.link = item_node.child("link").text().as_string();
            item.description = item_node.child("description").text().as_string();

            feed.items.push_back(item);
        }
        
        return feed;
    }
}