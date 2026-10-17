#pragma once

#include "feed_parser.h"
#include <memory>

namespace core{
    class FeedParserFactory{
        public:
        static std::unique_ptr<FeedParser> create(const std::string& xml_content);

    };
}