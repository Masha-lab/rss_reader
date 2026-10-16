cmake_minimum_required(VERSION 3.20)
project(RSSReader LANGUAGES CXX)

# Требование задания: C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Требование задания: строгие предупреждения
add_compile_options(-Wall -Wextra -Werror -pedantic)

# Санитайзеры для отладки (Address и Undefined Behavior)
# Включаются только в Debug-режиме
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -fno-omit-frame-pointer")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address,undefined")

# Подключаем три слоя архитектуры
# Порядок важен: зависимости идут сверху вниз (TUI -> App -> Core)
add_subdirectory(src/core)
add_subdirectory(src/app)
add_subdirectory(src/tui)

# Главный исполняемый файл
add_executable(rss_reader main.cpp)
target_link_libraries(rss_reader PRIVATE tui app core)