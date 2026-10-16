add_library(core STATIC feed.cpp)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Подключаем pugixml
find_package(pugixml REQUIRED)
target_link_libraries(core PRIVATE pugixml)