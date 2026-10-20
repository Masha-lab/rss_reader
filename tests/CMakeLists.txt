include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)

# Для Windows: предотвращаем переопределение переменных компилятора
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Тесты для ядра
add_executable(core_tests
    test_rss_parser.cpp
    test_atom_parser.cpp
    test_html_to_text.cpp
)

target_link_libraries(core_tests PRIVATE core gtest gtest_main)
target_include_directories(core_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Регистрация тестов в CTest
include(GoogleTest)
gtest_discover_tests(core_tests)
