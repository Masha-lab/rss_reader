#include <gtest/gtest.h>
#include "core/rss_parser.h"
#include "core/feed_parser_factory.h"

TEST(RSSParserTest, ParseValidRSS) {
    std::string rss_xml = R"(
<?xml version="1.0" encoding="UTF-8"?>
<rss version="2.0">
  <channel>
    <title>Test Feed</title>
    <link>https://example.com</link>
    <description>Test Description</description>
    <item>
      <title>Article 1</title>
      <link>https://example.com/article1</link>
      <description>&lt;p&gt;First article description&lt;/p&gt;</description>
      <guid>guid-1</guid>
    </item>
    <item>
      <title>Article 2</title>
      <link>https://example.com/article2</link>
      <description>&lt;p&gt;Second article description&lt;/p&gt;</description>
      <guid>guid-2</guid>
    </item>
  </channel>
</rss>
)";

    auto parser = core::FeedParserFactory::create(rss_xml);
    ASSERT_NE(parser, nullptr);
    
    auto feed = parser->parse(rss_xml);
    
    EXPECT_EQ(feed.title, "Test Feed");
    EXPECT_EQ(feed.items.size(), 2);
    EXPECT_EQ(feed.items[0].title, "Article 1");
    EXPECT_EQ(feed.items[1].title, "Article 2");
}

TEST(RSSParserTest, ParseEmptyRSS) {
    std::string rss_xml = R"(
<?xml version="1.0" encoding="UTF-8"?>
<rss version="2.0">
  <channel>
    <title>Empty Feed</title>
  </channel>
</rss>
)";

    auto parser = core::FeedParserFactory::create(rss_xml);
    ASSERT_NE(parser, nullptr);
    
    auto feed = parser->parse(rss_xml);
    
    EXPECT_EQ(feed.title, "Empty Feed");
    EXPECT_EQ(feed.items.size(), 0);
}

TEST(RSSParserTest, FactoryReturnsNullForInvalidXML) {
    std::string invalid_xml = "This is not XML at all";
    
    auto parser = core::FeedParserFactory::create(invalid_xml);
    EXPECT_EQ(parser, nullptr);
}
