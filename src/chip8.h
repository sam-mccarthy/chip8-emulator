#pragma once

#include <string>

class Chip8 {
public:
    Chip8();
    Chip8(const std::string& filename);

    ~Chip8();

    void load_file(const std::string& filename);
    void run();
};