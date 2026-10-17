#include "feed_parser_factory.h"
#include "rss_parser.h"
#include "atom_parser.h"
#include <pugixml.hpp>

namespace core{
    std::unique_ptr<FeedParser> FeedParserFactory::create(const std::string& xml_content){

        pugi::xml_document doc;
        doc.load_string(xml_content.c_str());

        if (doc.child("rss")){
            return std::make_unique<RssParser>();

        }

        if (doc.child("feed")){
            return std::make_unique<AtomParser>();

        }

        return nullptr;
    }
}