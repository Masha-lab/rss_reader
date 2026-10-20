add_library(tui STATIC tui_app.cpp)
target_include_directories(tui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(tui PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tui PUBLIC app)

find_package(FTXUI QUIET)
if(NOT FTXUI_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
        GIT_TAG v5.0.0
    )
    set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(FTXUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(FTXUI_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(ftxui)
endif()

target_link_libraries(tui PUBLIC ftxui::screen ftxui::dom ftxui::component)
