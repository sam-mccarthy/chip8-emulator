#include "system.h"
#include <format>
#include <iostream>
#include <map>

System::System(const uint8_t* program, size_t size) {

}

void System::run_cycle() {
    // Create opcode from two adjacent bytes in memory
    uint16_t opcode = _memory[_program_counter] << 8 | _memory[_program_counter + 1];

    // Referring to the last 3 bytes of the opcode, 0NNN
    uint8_t NNN = opcode & 0xFFF;
    uint8_t NN = opcode & 0xFF;
    uint8_t N = opcode & 0xFF;

    // Pointers referencing registers within the opcode, assuming there are registers referenced
    uint8_t* X = _registers + ((NNN - NN) >> 8);
    uint8_t* Y = _registers + ((NN - N) >> 4);

    std::map<uint16_t, std::function<void(void)>> functions = {
            {0x00E0, [&](){ std::fill(std::begin(_display_buffer), std::end(_display_buffer), 0); }},
            {0x00EE, [&](){
                if(_stack.empty()) std::cout<<"";
                _program_counter = _stack.front();
                _stack.pop_front();
                _program_counter--;
            }},
            {0x1000, [&](){ _program_counter = NNN - 1; }},
            {0x2000, [&](){
                _stack.push_back(_program_counter);
                _program_counter = NNN - 1;
            }},
            {0x3000, [&](){ if(*X == NN) _program_counter++; }},
            {0x4000, [&](){ if(*X != NN) _program_counter++; }},
            {0x5000, [&](){ if(*X == *Y) _program_counter++; }},
            {0x6000, [&](){ *X = NN; }},
            {0x7000, [&](){ *X += NN; }},
            {0x8000, [&](){ *X = *Y; }},
            {0x8001, [&](){ *X = *X | *Y; }},
            {0x8002, [&](){ *X = *X & *Y; }},
            {0x8003, [&](){ *X = *X ^ *Y; }},
            {0x8004, [&](){ *X += *Y; }}, // need to handle carry flag
            {0x8005, [&](){ *X = *X - *Y; }},
            {0x8006, [&](){ *X = *Y; *X >>= 1; }},
            {0x8007, [&](){ *X = *Y - *X; }},
            {0x800E, [&](){ *X = *Y; *X <<= 1; }},
            {0x9000, [&](){ if(*X != *Y) _program_counter++; }},
            {0xA000, [&](){ _index_register = NNN; }},
            {0xB000, [&](){ _program_counter = NNN + _registers[0]; }},
            {0xC000, [&](){ *X = rand() & NN; }},
            {0xD000, [&](){  }},
            {0xE09E, [&](){}},
            {0xE0A1, [&](){}},
            {0xF007, [&](){}},
            {0xF00A, [&](){}},
            {0xF015, [&](){}},
            {0xF018, [&](){}},
            {0xF01E, [&](){}},
            {0xF029, [&](){}},
            {0xF033, [&](){}},
            {0xF055, [&](){}},
            {0xF065, [&](){}}
    };

    _program_counter++;
}