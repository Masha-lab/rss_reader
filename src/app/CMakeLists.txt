add_library(app STATIC app_manager.cpp)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app PUBLIC core)

find_package(CURL REQUIRED)
target_link_libraries(app PRIVATE CURL::libcurl)