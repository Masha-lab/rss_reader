#include "atom_parser.h"
#include <pugixml.hpp>

namespace core{
    Feed AtomParser::parse(const std::string& xml_content){
        Feed feed;

        pugi::xml_document doc;
        doc.load_string(xml_content.c_str());

        pugi::xml_node root = doc.child("feed");
        feed.title = root.child("title").text().as_string();

        for(pugi::xml_node entry_node : root.children("entry")){
            Item item;
            item.title = entry_node.child("title").text().as_string();
            item.link = entry_node.child("link").attribute("href").as_string();
            item.description = entry_node.child("summary").text().as_string();
            feed.items.push_back(item);
        }
        return feed;
    }

}