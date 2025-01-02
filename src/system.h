#pragma once

#include <cstdint>
#include <string>
#include <deque>

class System {
public:
    System(const uint8_t* program, size_t size);
    void run_cycle();
private:
    uint8_t _memory[4096];
    uint8_t _registers[16];

    uint16_t _index_register;
    uint16_t _program_counter;

    // 64 x 32 display, where each bit is one pixel.
    // Stored column-major for drawing simplicity
    uint8_t* _display_buffer;

    uint8_t _display_width;
    uint8_t _display_height;
    uint8_t _display_size;

    uint8_t _delay_timer;
    uint8_t _sound_timer;
    bool _sound;

    std::deque<uint16_t> _stack;

    bool _keys[16];
    int _cycles;
};