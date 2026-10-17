#pragma once

#include "feed_parser.h"
#include <string>

namespace core{

    class AtomParser : public FeedParser{
    public:
    ~AtomParser() override = default;
    Feed parse(const std::string& xml_content) override;
    };
}