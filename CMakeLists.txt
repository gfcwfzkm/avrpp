cmake_minimum_required(VERSION 4.0)
project(avrpp VERSION 0.0.1 LANGUAGES CXX)

# Force C++23 but no compiler-specific tricks
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# I think I'll do header-only for now
add_library(avrpp_rtl INTERFACE)
target_include_directories(avrpp_rtl INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(avrpp_rtl INTERFACE -Wall -Wextra -Wpedantic)


add_executable(avrpp_dbg src/main.cpp)
target_link_libraries(avrpp_dbg PRIVATE avrpp_rtl)