#pragma once

#include <cstdint>
#include <string>

class System {
public:
    System(const std::string& filename);
    void run_cycle();
private:
    uint8_t _memory[4096];
    uint8_t _registers[16];

    uint16_t _index_register;
    uint16_t _program_counter;

    uint8_t _display_buffer[128 * 64];

    uint8_t _delay_timer;
    uint8_t _sound_timer;
    bool _sound;

    uint16_t _stack[16];

    bool _keys[16];
    int _cycles;
};