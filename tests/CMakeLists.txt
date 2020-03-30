include(${PROJECT_SOURCE_DIR}/cmake/Catch2.cmake)

add_executable(test_all
  test_all.cpp
  percy/parser.cpp
  percy/result.cpp
  percy/static_input.cpp
  percy/type_traits.cpp
)

target_link_libraries(test_all Percy Catch2::Catch2)

if(${PERCY_RUNTIME_TESTS} STREQUAL ON)
  add_definitions(-DRUNTIME_TESTS)
endif()
