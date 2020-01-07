cmake_minimum_required(VERSION 3.8)
project(RichLogging)

# =========================================
# global options

option(RICH_LOG_FORCE_MACRO_PREFIX "if true, only RICH_ macro versions are available" OFF)


# =========================================
# define library

file(GLOB_RECURSE SOURCES "src/*.cc" "src/*.hh")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${SOURCES})

add_library(rich-log STATIC ${SOURCES})

target_include_directories(rich-log PUBLIC src/)

target_link_libraries(rich-log PUBLIC
    clean-core
    reflector
)


# =========================================
# set up compile flags

if (MSVC)
    target_compile_options(rich-log PUBLIC /MP)
else()
    target_compile_options(rich-log PRIVATE -Wall -fPIC)
    target_link_libraries(rich-log PUBLIC -fuse-ld=gold)
endif()

if (RICH_LOG_FORCE_MACRO_PREFIX)
    target_compile_definitions(rich-log PUBLIC RICH_LOG_FORCE_MACRO_PREFIX)
endif()
