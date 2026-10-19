add_library(core STATIC 
    feed.cpp
    rss_parser.cpp 
    atom_parser.cpp 
    feed_parser_factory.cpp 
    html_to_text.cpp
    paragraph_handler.cpp 
    heading_handler.cpp
    list_handler.cpp
    list_item_handler.cpp
    link_handler.cpp 
    html_to_text_converter.cpp 
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Подключаем pugixml
find_package(pugixml REQUIRED)
target_link_libraries(core PRIVATE pugixml)