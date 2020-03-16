include_directories(${percy_SOURCE_DIR}/include/ include/)

file(READ input.txt CODE)

add_definitions(-DINPUT_CODE="${CODE}")

add_executable(example src/example.cpp)
