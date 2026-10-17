#include <iostream>
#include "feed_parser_factory.h"

int main() {
    std::cout << "RSS Reader v0.2 - test factory\n";
    
    std::string rss_xml = R"(
        <rss>
            <channel>
                <title>BBC News</title>
                <item>
                    <title>Новость 1</title>
                    <link>https://bbc.com/1</link>
                    <description>Описание 1</description>
                </item>
            </channel>
        </rss>
    )";
    if (parser){

    
    core::Feed feed = parser->parser(rss_xml);
    feed.title = "Тестовая лента";
    
    std::cout << "Лента: " << feed.title << "\n";
    std::cout << "Статей: " << feed.items.size() << "\n";
    }else{
        std::cout << "mistake\n";
    }
    return 0;
}
