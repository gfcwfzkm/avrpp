# Figuring out testing...
include(FetchContent)
FetchContent_Declare(doctest
    GIT_REPOSITORY  https://github.com/doctest/doctest.git
    GIT_TAG         v2.4.12
    GIT_SHALLOW     TRUE
    SYSTEM)
FetchContent_MakeAvailable(doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

add_executable(avrpp_tests
    test_main.cpp
)
target_link_libraries(avrpp_tests PRIVATE avrpp_rtl doctest::doctest)

doctest_discover_tests(avrpp_tests)