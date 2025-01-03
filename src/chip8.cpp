#include "chip8.h"
#include "system.h"
#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_mixer.h>

Chip8::Chip8(const std::string &filename) {
    _program = nullptr;
    _program_size = 0;
    load_file(filename);
}

Chip8::~Chip8() {
    delete _program;
}

bool Chip8::load_file(const std::string &filename) {
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
        return false;
    }

    return true;
}

bool Chip8::run() {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Window* window = SDL_CreateWindow("chip8 emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
    if(!window){
        std::cout << "Error creating window: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    if(!surface){
        std::cout << "Error getting window surface" << SDL_GetError() << std::endl;
        return false;
    }

    System system(_program, _program_size);
    while(true){
        if(!handle_input(system)) break;
        //system.run_cycle();
        draw_screen(system);
        // We'll want to handle SDL things in the future.
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Chip8::handle_input(System& system) {
    const int SCAN_CODES[16] = {
            SDL_SCANCODE_1,
            SDL_SCANCODE_2,
            SDL_SCANCODE_3,
            SDL_SCANCODE_4,
            SDL_SCANCODE_Q,
            SDL_SCANCODE_W,
            SDL_SCANCODE_E,
            SDL_SCANCODE_R,
            SDL_SCANCODE_A,
            SDL_SCANCODE_S,
            SDL_SCANCODE_D,
            SDL_SCANCODE_F,
            SDL_SCANCODE_Z,
            SDL_SCANCODE_X,
            SDL_SCANCODE_C,
            SDL_SCANCODE_V
    };

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: return false;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                const int* position = std::find(std::begin(SCAN_CODES), std::end(SCAN_CODES), event.key.keysym.scancode);
                if(position != std::end(SCAN_CODES))
                    system.set_key(std::distance(SCAN_CODES, position), event.type == SDL_KEYDOWN);
                break;
        }
    }

    return true;
}

void Chip8::draw_screen(System& system) {

}