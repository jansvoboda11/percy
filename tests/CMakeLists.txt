include(FetchContent)

FetchContent_Declare(catch2
        GIT_REPOSITORY "https://github.com/catchorg/Catch2"
        GIT_TAG "v2.11.1"
        )

FetchContent_GetProperties(catch2)

if (NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
    include_directories(${catch2_SOURCE_DIR}/single_include/)
endif ()

include_directories(${percy_SOURCE_DIR}/include/)

add_executable(test_all
        test_all.cpp
        percy/parser.cpp
        percy/result.cpp
        percy/static_input.cpp
        )
