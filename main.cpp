#include <iostream>
#include <cstdlib>
#include "app/app_manager.h"
#include "core/html_to_text_converter.h"
#include "core/feed_parser_factory.h"

int main() {
    // Отключаем вывод об утечках системной библиотеки
    setenv("LSAN_OPTIONS", "detect_leaks=0", 1);
    
    std::cout << "=== Тест 1: Парсинг RSS ===\n";
    
    std::string rss_xml = R"(
        <rss version="2.0">
            <channel>
                <title>Тестовая лента</title>
                <item>
                    <title>Новость 1</title>
                    <link>https://example.com/1</link>
                    <description>Описание первой новости</description>
                </item>
                <item>
                    <title>Новость 2</title>
                    <link>https://example.com/2</link>
                    <description>Описание второй новости</description>
                </item>
            </channel>
        </rss>
    )";
    
    auto parser = core::FeedParserFactory::create(rss_xml);
    if (parser) {
        auto feed = parser->parse(rss_xml);
        std::cout << "Название: " << feed.title << "\n";
        std::cout << "Статей: " << feed.items.size() << "\n";
        for (const auto& item : feed.items) {
            std::cout << "  - " << item.title << "\n";
        }
    } else {
        std::cout << "Ошибка: парсер не создан\n";
    }
    
    std::cout << "\n=== Тест 2: HTML-to-Text ===\n";
    
    std::string html = R"(
        <div>
            <h1>Заголовок статьи</h1>
            <p>Это первый абзац с <a href="https://example.com">ссылкой</a>.</p>
            <ul>
                <li>Пункт 1</li>
                <li>Пункт 2</li>
            </ul>
        </div>
    )";
    
    core::HtmlToTextConverter converter;
    auto result = converter.convert(html);
    std::cout << result.text << "\n";
    
    std::cout << "\n=== Тест 3: Загрузка по сети (опционально) ===\n";
    std::cout << "Если хочешь протестировать сеть, раскомментируй код в main.cpp\n";
    
    return 0;
}