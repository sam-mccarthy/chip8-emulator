#include "system.h"
#include <format>
#include <iostream>

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

    typedef struct {
        std::string Code;
        std::function<void(uint16_t)> Handler;
    } CodeHandler functions[35] = {
            {"00E0", [&](uint16_t opc){ std::fill(std::begin(_display_buffer), std::end(_display_buffer), 0); }},
            {"00EE", [&](uint16_t opc){
                if(_stack.empty()) std::cout<<"";
                _program_counter = _stack.front();
                _stack.pop_front();
                _program_counter--;
            }},
            {"1NNN", [&](uint16_t opc){ _program_counter = NNN - 1; }},
            {"2NNN", [&](uint16_t opc){
                _stack.push_back(_program_counter);
                _program_counter = NNN - 1;
            }},
            {"3XNN", [&](uint16_t opc){ if(*X == NN) _program_counter++; }},
            {"4XNN", [&](uint16_t opc){ if(*X != NN) _program_counter++; }},
            {"5XY0", [&](uint16_t opc){ if(*X == *Y) _program_counter++; }},
            {"6XNN", [&](uint16_t opc){ *X = NN; }},
            {"7XNN", [&](uint16_t opc){ *X += NN; }},
            {"8XY0", [&](uint16_t opc){ *X = *Y; }},
            {"8XY1", [&](uint16_t opc){ *X = *X | *Y; }},
            {"8XY2", [&](uint16_t opc){ *X = *X & *Y; }},
            {"8XY3", [&](uint16_t opc){ *X = *X ^ *Y; }},
            {"8XY4", [&](uint16_t opc){ *X += *Y; }}, // need to handle carry flag
            {"8XY5", [&](uint16_t opc){ *X = *X - *Y; }},
            {"8XY6", [&](uint16_t opc){ *X = *Y; *X >>= 1; }},
            {"8XY7", [&](uint16_t opc){ *X = *Y - *X; }},
            {"8XYE", [&](uint16_t opc){ *X = *Y; *X <<= 1; }},
            {"9XY0", [&](uint16_t opc){ if(*X != *Y) _program_counter++; }},
            {"ANNN", [&](uint16_t opc){ _index_register = NNN; }},
            {"BNNN", [&](uint16_t opc){ _program_counter = NNN + _registers[0]; }},
            {"CXNN", [&](uint16_t opc){ *X = rand() & NN; }},
            {"DXYN", [&](uint16_t opc){}},
            {"EX9E", [&](uint16_t opc){}},
            {"EXA1", [&](uint16_t opc){}},
            {"FX07", [&](uint16_t opc){}},
            {"FX0A", [&](uint16_t opc){}},
            {"FX15", [&](uint16_t opc){}},
            {"FX18", [&](uint16_t opc){}},
            {"FX1E", [&](uint16_t opc){}},
            {"FX29", [&](uint16_t opc){}},
            {"FX33", [&](uint16_t opc){}},
            {"FX55", [&](uint16_t opc){}},
            {"FX65", [&](uint16_t opc){}}
    };

    _program_counter++;
}