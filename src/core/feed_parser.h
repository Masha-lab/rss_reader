#pragma once

#include "feed.h"
#include <string>

namespace core{
    
    class FeedParser{
        public:
        virtual ~FeedParser() = default;
        virtual Feed parse(const std::string& xml_content) = 0;

    };
}