#include <iostream>
#include "feed.h"

int main() {
    std::cout << "RSS Reader v0.1\n";
    
    core::Feed feed;
    feed.title = "Тестовая лента";
    
    std::cout << "Лента: " << feed.title << "\n";
    std::cout << "Статей: " << feed.items.size() << "\n";
    
    return 0;
}
