#pragma once

#include "system.h"
#include <string>

class Chip8 {
public:
    Chip8() = default;
    Chip8(const std::string& filename);

    ~Chip8();

    bool load_file(const std::string& filename);
    bool run();

    bool handle_input(System& system);
    void draw_screen(System& system);
private:
    uint8_t* _program;
    size_t _program_size;
};