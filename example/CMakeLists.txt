file(READ input.txt CODE)

add_definitions(-DINPUT_CODE="${CODE}")

include_directories(include/)

add_executable(example src/example.cpp)

target_link_libraries(example Percy)
