cmake_minimum_required(VERSION 3.29)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 20)
set(SRC_FILES src/main.cpp src/system.cpp src/chip8.cpp src/octo.cpp)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(SDL2_mixer REQUIRED)

add_executable(chip8_emulator ${SRC_FILES})

if(TARGET SDL2::SDL2main)
    target_link_libraries(chip8_emulator PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(chip8_emulator PRIVATE SDL2::SDL2)
target_link_libraries(chip8_emulator PRIVATE SDL2_mixer::SDL2_mixer)