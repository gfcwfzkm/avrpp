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
    test_regfile.cpp
)
target_link_libraries(avrpp_tests PRIVATE avrpp_rtl doctest::doctest)

# AddressSanitizer + UBSan: an out-of-bounds register index fails the test instead of passing by luck
target_compile_options(avrpp_tests PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
target_link_options(avrpp_tests PRIVATE -fsanitize=address,undefined)

doctest_discover_tests(avrpp_tests)