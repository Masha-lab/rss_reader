#include <gtest/gtest.h>
#include "core/atom_parser.h"
#include "core/feed_parser_factory.h"

TEST(AtomParserTest, ParseValidAtom) {
    std::string atom_xml = R"(
<?xml version="1.0" encoding="UTF-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
  <title>Atom Test Feed</title>
  <link href="https://example.com"/>
  <entry>
    <title>Atom Article 1</title>
    <link href="https://example.com/atom1"/>
    <summary>First Atom article</summary>
    <id>atom-guid-1</id>
  </entry>
  <entry>
    <title>Atom Article 2</title>
    <link href="https://example.com/atom2"/>
    <summary>Second Atom article</summary>
    <id>atom-guid-2</id>
  </entry>
</feed>
)";

    auto parser = core::FeedParserFactory::create(atom_xml);
    ASSERT_NE(parser, nullptr);
    
    auto feed = parser->parse(atom_xml);
    
    EXPECT_EQ(feed.title, "Atom Test Feed");
    EXPECT_EQ(feed.items.size(), 2);
    EXPECT_EQ(feed.items[0].title, "Atom Article 1");
    EXPECT_EQ(feed.items[1].title, "Atom Article 2");
}

TEST(AtomParserTest, ParseEmptyAtom) {
    std::string atom_xml = R"(
<?xml version="1.0" encoding="UTF-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
  <title>Empty Atom Feed</title>
</feed>
)";

    auto parser = core::FeedParserFactory::create(atom_xml);
    ASSERT_NE(parser, nullptr);
    
    auto feed = parser->parse(atom_xml);
    
    EXPECT_EQ(feed.title, "Empty Atom Feed");
    EXPECT_EQ(feed.items.size(), 0);
}
