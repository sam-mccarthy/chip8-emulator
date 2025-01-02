#pragma once

#include <string>

class Chip8 {
public:
    Chip8() = default;
    Chip8(const std::string& filename);

    ~Chip8();

    void load_file(const std::string& filename);
    void run();
private:
    uint8_t* _program;
    size_t _program_size;
};