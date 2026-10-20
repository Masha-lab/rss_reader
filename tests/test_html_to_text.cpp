#include <gtest/gtest.h>
#include "core/html_to_text_converter.h"

TEST(HtmlToTextTest, ConvertSimpleParagraph) {
    core::HtmlToTextConverter converter;
    std::string html = "<p>Hello World</p>";
    
    auto result = converter.convert(html);
    
    // Конвертер добавляет переводы строк вокруг блочных элементов
    EXPECT_TRUE(result.text.find("Hello World") != std::string::npos);
}

TEST(HtmlToTextTest, ConvertMultipleParagraphs) {
    core::HtmlToTextConverter converter;
    std::string html = "<p>First paragraph</p><p>Second paragraph</p>";
    
    auto result = converter.convert(html);
    
    EXPECT_TRUE(result.text.find("First paragraph") != std::string::npos);
    EXPECT_TRUE(result.text.find("Second paragraph") != std::string::npos);
}

TEST(HtmlToTextTest, ConvertLinks) {
    core::HtmlToTextConverter converter;
    std::string html = "<p>Visit <a href=\"https://example.com\">Example</a> for more info</p>";
    
    auto result = converter.convert(html);
    
    EXPECT_TRUE(result.text.find("Example") != std::string::npos);
    EXPECT_EQ(result.links.size(), 1);
    EXPECT_EQ(result.links[0], "https://example.com");
}

TEST(HtmlToTextTest, ConvertHeadings) {
    core::HtmlToTextConverter converter;
    std::string html = "<h1>Main Title</h1><h2>Subtitle</h2><p>Content</p>";
    
    auto result = converter.convert(html);
    
    EXPECT_TRUE(result.text.find("Main Title") != std::string::npos);
    EXPECT_TRUE(result.text.find("Subtitle") != std::string::npos);
    EXPECT_TRUE(result.text.find("Content") != std::string::npos);
}

TEST(HtmlToTextTest, ConvertEmptyHTML) {
    core::HtmlToTextConverter converter;
    std::string html = "";
    
    auto result = converter.convert(html);
    
    EXPECT_TRUE(result.text.empty());
    EXPECT_EQ(result.links.size(), 0);
}

TEST(HtmlToTextTest, ConvertHTMLWithEntities) {
    core::HtmlToTextConverter converter;
    std::string html = "<p>Price: &lt;100&gt; &amp; &quot;special&quot;</p>";
    
    auto result = converter.convert(html);
    
    EXPECT_TRUE(result.text.find("<100>") != std::string::npos);
    EXPECT_TRUE(result.text.find("&") != std::string::npos);
    EXPECT_TRUE(result.text.find("\"special\"") != std::string::npos);
}
