
cmake_minimum_required(VERSION 3.15)

project(chip8)

set(CMAKE_CXX_STANDARD 17)
find_package(SFML 2.5 
COMPONENTS 
system window graphics audio REQUIRED)
include_directories(${chip8_SOURCE_DIR}/include)

set(SOURCE_FILES
    ../src/graphics.cpp
    ../src/chip8.cpp
    ../src/main.cpp)


add_executable(emulator ${SOURCE_FILES})
target_link_libraries(emulator sfml-graphics)

install(TARGETS emulator DESTINATION bin)
