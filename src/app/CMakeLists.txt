add_library(app STATIC
    app_manager.cpp
    json_state_storage.cpp
)

target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(app PUBLIC core)

find_package(CURL REQUIRED)
target_link_libraries(app PRIVATE CURL::libcurl)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(app PUBLIC nlohmann_json::nlohmann_json)
