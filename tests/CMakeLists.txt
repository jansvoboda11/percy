include(FetchContent)

FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2
  GIT_TAG v2.11.1
)

FetchContent_MakeAvailable(Catch2)

include_directories(${percy_SOURCE_DIR}/include/ ${Catch2_SOURCE_DIR}/single_include/)

add_executable(test_all
  test_all.cpp
  percy/parser.cpp
  percy/result.cpp
  percy/static_input.cpp
  percy/utils.cpp
)
