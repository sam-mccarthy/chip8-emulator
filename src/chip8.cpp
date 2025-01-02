#include "chip8.h"
#include "system.h"
#include <iostream>
#include <fstream>

Chip8::Chip8(const std::string &filename) {
    _program = nullptr;
    _program_size = 0;
    load_file(filename);
}

Chip8::~Chip8() {
    delete _program;
}

void Chip8::load_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if(file.is_open()){
        file.seekg(0, std::ios_base::end);
        // The memory of the emulator is limited to 0x1000, so
        // we'll only allow files up to that limit, excluding fonts.
        _program_size = std::min(0x1000 - 0x200, (int)file.tellg());
        file.seekg(0, std::ios_base::beg);

        _program = new uint8_t[_program_size];
        file.read((char*)_program, _program_size);
    } else {
        std::cerr << "Error opening file.";
    }
}

void Chip8::run() {
    bool exit = false;
    System system(_program, _program_size);
    while(!exit){
        system.run_cycle();
        // We'll want to handle SDL things in the future.

        exit = true;
    }
}
