#include "chip8.h"

int main() {
    Chip8 emulator("pic.ch8");
    emulator.run();
    return 0;
}
