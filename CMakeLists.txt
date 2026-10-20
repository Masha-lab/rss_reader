cmake_minimum_required(VERSION 3.16)
project(rss_reader VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Отключаем предупреждения как ошибки для тестов
option(BUILD_TESTS "Build unit tests" ON)

# Основные библиотеки
add_subdirectory(src/core)
add_subdirectory(src/app)
add_subdirectory(src/tui)

# Исполняемый файл
add_executable(rss_reader main.cpp)
target_link_libraries(rss_reader PRIVATE core app tui)

# Unit-тесты
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
