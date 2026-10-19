cmake_minimum_required(VERSION 3.16)
project(rss_reader CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

add_subdirectory(src/core)
add_subdirectory(src/app)
add_subdirectory(src/tui)

find_package(CURL REQUIRED)

add_executable(rss_reader main.cpp)
target_link_libraries(rss_reader PRIVATE app tui core CURL::libcurl)
target_include_directories(rss_reader PRIVATE ${CMAKE_SOURCE_DIR}/src)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(app PUBLIC nlohmann_json::nlohmann_json)
