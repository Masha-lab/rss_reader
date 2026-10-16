add_library(tui STATIC tui_app.cpp)
target_include_directories(tui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tui PUBLIC app)
