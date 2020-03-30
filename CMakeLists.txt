cmake_minimum_required(VERSION 3.15)

project(Percy)

set(CMAKE_CXX_STANDARD 20)

add_library(Percy INTERFACE)

option(PERCY_BUILD_EXAMPLE "Build example project build with Percy." OFF)
option(PERCY_BUILD_TESTS "Build tests of Percy." OFF)
option(PERCY_RUNTIME_TESTS "Evaluate tests of Percy at run-time instead of compile-time." OFF)

include(cmake/PercyVariant.cmake)

target_link_libraries(Percy INTERFACE Percy::Variant)

target_include_directories(
  Percy
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(PERCY_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Percy")

include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/PercyConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/PercyConfig.cmake
  INSTALL_DESTINATION
  ${PERCY_CMAKE_CONFIG_DESTINATION}
)

install(
  TARGETS
    Percy
  EXPORT
    PercyTargets
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT
    PercyTargets
  NAMESPACE
    Percy::
  DESTINATION
    ${PERCY_CMAKE_CONFIG_DESTINATION}
)

install(
  DIRECTORY
    "include/"
  DESTINATION
    "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PercyConfig.cmake"
  DESTINATION
    ${PERCY_CMAKE_CONFIG_DESTINATION}
)

if(${PERCY_BUILD_EXAMPLE} STREQUAL ON)
  add_subdirectory(example)
endif()

if(${PERCY_BUILD_TESTS} STREQUAL ON)
  add_subdirectory(tests)
endif()
