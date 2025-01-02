#include "chip8.h"
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>

int main() {
    Chip8 emulator("pic.ch8");
    emulator.run();
    return 0;
}
