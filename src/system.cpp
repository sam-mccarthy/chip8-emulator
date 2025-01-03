#include "system.h"
#include <iostream>
#include <iomanip>
#include <map>

System::System(const uint8_t* program, size_t program_size) {
    _program_counter = 0x200;

    _display_width = 64;
    _display_height = 32;
    _display_size = _display_width / 8 * _display_height;

    _display_buffer = new uint8_t[_display_size];

    _index_register = 0;
    _delay_timer = 0;
    _sound_timer = 0;
    _sound = false;

    _cycles = 0;

    memset(_keys, 0, 16);
    memset(_memory, 0, 4096);
    memset(_registers, 0, 16);
    memset(_display_buffer, 0, _display_size);
    uint8_t font[5 * 16] = {0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                            0x20, 0x60, 0x20, 0x20, 0x70, // 1
                            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                            0xF0, 0x80, 0xF0, 0x80, 0x80}; // F
    memcpy(_memory + 0x50, font, 5 * 16);
    memcpy(_memory + 0x200, program, program_size);
}

bool System::run_cycle() {
    // Create opcode from two adjacent bytes in memory
    uint16_t opcode = _memory[_program_counter] << 8 | _memory[_program_counter + 1];

    // Referring to the last 3 bytes of the opcode, 0NNN
    uint8_t NNN = opcode & 0xFFF;
    uint8_t NN = opcode & 0xFF;
    uint8_t N = opcode & 0xFF;

    // Pointers referencing registers within the opcode, assuming there are registers referenced
    uint8_t* X = _registers + ((NNN - NN) >> 8);
    uint8_t* Y = _registers + ((NN - N) >> 4);

    std::map<uint16_t, std::function<void(void)>> handlers = {
            {0x00E0, [&](){ std::fill(_display_buffer, _display_buffer + _display_size, 0); }},
            {0x00EE, [&](){
                if(_stack.empty()) std::cout<<"";
                _program_counter = _stack.front();
                _stack.pop_front();
                _program_counter -= 2;
            }},
            {0x1000, [&](){ _program_counter = NNN - 2; }},
            {0x2000, [&](){
                _stack.push_back(_program_counter);
                _program_counter = NNN - 2;
            }},
            {0x3000, [&](){ if(*X == NN) _program_counter += 2; }},
            {0x4000, [&](){ if(*X != NN) _program_counter += 2; }},
            {0x5000, [&](){ if(*X == *Y) _program_counter += 2; }},
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
            {0x9000, [&](){ if(*X != *Y) _program_counter += 2; }},
            {0xA000, [&](){ _index_register = NNN; }},
            {0xB000, [&](){ _program_counter = NNN + _registers[0]; }},
            {0xC000, [&](){ *X = rand() & NN; }},
            {0xD000, [&](){
                uint8_t x = *X % _display_width;
                // Our display is stored as bits, so we need to offset if we happen to draw to two bytes at once.
                uint8_t offset = *X % 8;

                uint8_t y = *Y % _display_height;
                for(int i = 0; i < N; i++){
                    if(offset == 0) _display_buffer[(x / 8) * _display_height + y + i] ^= _memory[_index_register + i];
                    _display_buffer[x * _display_height + y + i] ^= _memory[_index_register + i] >> offset;
                    if(x + 8 <= _display_width)
                        _display_buffer[(x / 8 + 1) * _display_height + y + i] ^= _memory[_index_register + i] << (8 - offset);
                }
            }},
            {0xE09E, [&](){ if(_keys[*X]) _program_counter += 2; }},
            {0xE0A1, [&](){ if(!_keys[*X]) _program_counter += 2; }},
            {0xF007, [&](){ *X = _delay_timer; }},
            {0xF00A, [&](){ if(!_keys[*X]) _program_counter -= 2; }},
            {0xF015, [&](){ _delay_timer = *X; }},
            {0xF018, [&](){ _sound_timer = *X; }},
            {0xF01E, [&](){ _index_register += *X; }},
            {0xF029, [&](){ _index_register = 0x50 + 5 * *X;}},
            {0xF033, [&](){
                _memory[_index_register] = *X / 100;
                _memory[_index_register + 1] = (*X % 100) / 10;
                _memory[_index_register + 2] = *X % 10;
            }},
            {0xF055, [&](){ memcpy(_memory + _index_register, _registers, *X + 1); }},
            {0xF065, [&](){ memcpy(_registers, _memory + _index_register, *X + 1); }}
    };

    _program_counter += 2;
    if(_program_counter > 4096)
        _program_counter = 0;

    uint16_t opc_mask_nnn = opcode & 0xF000;
    uint16_t opc_mask_xy = opcode & 0xF00F;
    uint16_t opc_mask_x = opcode & 0xF0FF;

    //std::cout << std::setfill('0') << std::setw(4) << std::hex << (int)opcode << std::endl;
    if(handlers.contains(opc_mask_x)) {
        //std::cout << std::setfill('0') << std::setw(4) << std::hex << (int)opc_mask_x << std::endl;
        handlers[opc_mask_x]();
    }
    else if(handlers.contains(opc_mask_xy)) {
        //std::cout << std::setfill('0') << std::setw(4) << std::hex << (int)opc_mask_xy << std::endl;
        handlers[opc_mask_xy]();
    }
    else if(handlers.contains(opc_mask_nnn)) {
        //std::cout << std::setfill('0') << std::setw(4) << std::hex << (int)opc_mask_nnn << std::endl;
        handlers[opc_mask_nnn]();
    }
    else {
        std::cout << "Failed processing instruction " << std::setfill('0') << std::setw(4) << std::hex << (int) opcode
                  << std::endl;
    }

    return false;
}

void System::set_key(int index, bool state) {
    if(index >= 0 && index < 16)
        _keys[index] = state;
}
