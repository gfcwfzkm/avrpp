#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("Test 1 to see if it passes") {
    CHECK_FALSE(false);
}

TEST_CASE("Test 2, should fail!") {
    CHECK_FALSE(true);
}