#pragma once

#include <cstdint>
#include <array>

class PPU {
public:
    // PPU Registers
    uint8_t STAT = 0x00;
    uint8_t SCY = 0x00;
    uint8_t SCX = 0x00;
    uint8_t LY = 0x00;
    uint8_t LYC = 0x00;
    uint8_t DMA = 0x00;
    uint8_t BGP = 0x00;
    uint8_t OBP0 = 0x00;
    uint8_t OBP1 = 0x00;
    uint8_t WY = 0x00;
    uint8_t WX = 0x00;

    //LCDC Register Flags
    bool lcd_ppu_enable = false;
    bool window_tile_map_area = false; //false-9800-9BFF true-9C00-9FFF
    bool window_enable = false;
    bool bg_window_tile_data_area = false; //false-8800-97FF true-8000-8FFF
    bool bg_tile_map_area = false; //false-9800-9BFF true-9C00-9FFF
    bool obj_size = false; //false-8x8 true 8x16
    bool obj_enable = false;
    bool bg_window_enable_priority = false;

    bool v_mode = 2;

    unsigned char framebuffer[160 * 144 * 3] = { 0x00 };

    PPU()
    {
    }

private:
    //void updateRegisters();
};