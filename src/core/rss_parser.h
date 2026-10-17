#pragma once

#include "feed_parser.h"
#include <string>

namespace core {

    class RssParser : public FeedParser{
        public:
        ~RssParser() override = default;
        Feed parse(const std::string& xml_content) override;
    };
}