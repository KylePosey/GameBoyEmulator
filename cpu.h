#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <vector>
#include "ppu.h";
#include <stdio.h>
#include <cassert>
#include <cmath>
#include <bitset>

class CPU {
	
public:
	uint16_t sp = 0x0;
	uint16_t pc = 0x0;

	uint8_t a = 0x0;
	uint8_t b = 0x0;
	uint8_t c = 0x0;
	uint8_t d = 0x0;
	uint8_t e = 0x0;
	uint8_t h = 0x0;
	uint8_t l = 0x0;

	//flag bits
	bool z_flag = false;
	bool n_flag = false;
	bool h_flag = false;
	bool c_flag = false;
	bool IME_flag = false;
	bool deferredIME = false;
	int deferredIMECycles = 0;

	bool halt = false;

	uint8_t IF = 0x00;

	int m_cycles = 0;
	int last_m_cycles = 0;

	const uint8_t INTERRUPT_VBLANK = 0x01; // Bit 0
	const uint8_t INTERRUPT_LCD = 0x02; // Bit 1
	const uint8_t INTERRUPT_TIMER = 0x04; // Bit 2
	const uint8_t INTERRUPT_SERIAL = 0x08; // Bit 3
	const uint8_t INTERRUPT_JOYPAD = 0x10; // Bit 4

	std::vector<byte> full_rom;
	std::vector<byte> full_ram;
	std::array<byte, 0xFFFF + 1> ram = { 0 };

	uint8_t directionState = 0x0F;
	uint8_t actionState = 0x0F;

	PPU ppu;

	uint8_t mbc1_mode = 0; // Mode 0: 16Mbit ROM / 8KB RAM, Mode 1: 4Mbit ROM / 32KB RAM
	uint8_t mbc1_bank1 = 1; // 5-bit ROM bank number (bit 0-4)
	uint8_t mbc1_bank2 = 0; // 2-bit ROM/RAM bank number (bit 5-6)
	bool mbc1_ram_enable = false; // Whether external RAM is enabled

	int div_cycles = 0;      // Divider register cycles
	int timer_cycles = 0;    // Timer counter cycles

	int exitcycles = 0;
	bool zero = false;

	CPU()
	{
		full_ram.resize(0x8000);
	}

	uint8_t xy = 0;

	struct Sprite {
		uint8_t y;
		uint8_t x;
		uint8_t tileIndex;
		uint8_t attributes;
	};

	std::array<Sprite, 40> oam;

	void loadOAM() {
		for (int i = 0; i < 40; i++) {
			oam[i].y = readMemory(0xFE00 + i * 4);
			oam[i].x = readMemory(0xFE00 + i * 4 + 1);
			oam[i].tileIndex = readMemory(0xFE00 + i * 4 + 2);
			oam[i].attributes = readMemory(0xFE00 + i * 4 + 3);
		}
	}

	void performDMATransfer(uint8_t value) {
		// Only proceed if value is in range 0x00 - 0xF1 (transfers start from 0x0000 to 0xF100)
		if (value <= 0xF1) {
			uint16_t sourceAddress = value * 0x100;
			for (int i = 0; i < 0xA0; i++) {  // OAM is 160 bytes (40 sprites * 4 bytes each)
				writeMemory(0xFE00 + i, readMemory(sourceAddress + i));
			}
		}
	}

	void renderSprites() {
		uint8_t lcdc = readMemory(0xFF40);

		// Check if sprites are enabled
		if (!(lcdc & (1 << 1))) {
			return;
		}

		// Load OAM data
		loadOAM();

		for (int i = 0; i < 40; i++) {
			Sprite& sprite = oam[i];

			// Calculate actual screen positions
			int spriteX = sprite.x - 8;
			int spriteY = sprite.y - 16;

			// Skip sprite if it's off-screen
			if (spriteX < -7 || spriteX >= 160 || spriteY < -7 || spriteY >= 144) {
				continue;
			}

			// Get the sprite's tile data from VRAM
			uint16_t tileAddress = 0x8000 + (sprite.tileIndex * 16);  // 16 bytes per tile

			// Determine sprite size (8x8 or 8x16)
			bool sprite8x16 = lcdc & (1 << 2);
			int spriteHeight = sprite8x16 ? 16 : 8;

			for (int line = 0; line < spriteHeight; line++) {
				int y = spriteY + line;

				if (y < 0 || y >= 144) continue;  // Skip if outside vertical screen bounds

				// Handle vertical flipping
				int tileLine = (sprite.attributes & 0x40) ? (spriteHeight - 1 - line) : line;

				uint8_t lowerByte = readMemory(tileAddress + (tileLine * 2));
				uint8_t upperByte = readMemory(tileAddress + (tileLine * 2) + 1);

				for (int pixel = 7; pixel >= 0; pixel--) {
					int x = spriteX + (7 - pixel);

					if (x < 0 || x >= 160) continue;  // Skip if outside horizontal screen bounds

					// Handle horizontal flipping
					int bitIndex = (sprite.attributes & 0x20) ? pixel : (7 - pixel);

					// Get the color ID for the pixel
					uint8_t colorBit = ((upperByte >> bitIndex) & 0x1) << 1 | ((lowerByte >> bitIndex) & 0x1);

					// Apply the palette and render the pixel if not transparent (color 0 is transparent)
					if (colorBit != 0) {
						applyPaletteAndRenderPixel(x, y, colorBit, sprite.attributes);
					}
				}
			}
		}
	}

	// Helper function to apply the palette and render a pixel on the screen
	void applyPaletteAndRenderPixel(int x, int y, uint8_t colorBit, uint8_t attributes) {
		uint8_t palette = (attributes & 0x10) ? readMemory(0xFF49) : readMemory(0xFF48);  // OBP1 or OBP0

		// Extract the color from the palette
		uint8_t color = (palette >> (colorBit * 2)) & 0x03;

		// Convert the color to a grayscale value
		const uint8_t colors[4] = { 0xFF, 0xC0, 0x80, 0x00 };  // White, Light Gray, Dark Gray, Black
		uint8_t pixelColor = colors[color];

		// Calculate the index in the framebuffer
		int index = (143 - y) * 160 + x;

		// Render the pixel
		ppu.framebuffer[index] = pixelColor;
	}

	void parseROMHeader()
	{
		if (full_rom.size() < 0x0148)
		{
			printf("Error: ROM size (%lu bytes) is too small to contain a valid header!\n", full_rom.size());
			exit(-1);
		}

		uint8_t rom_size_code = full_rom[0x0148];
		uint8_t cart_type = full_rom[0x0147];

		uint32_t expected_rom_size = 32 * 1024 * (1 << rom_size_code); // 32KB * 2^code
		uint32_t detected_rom_size = full_rom.size();

		if (detected_rom_size != expected_rom_size) {
			printf("Warning: Detected ROM size (%u) does not match header size (%u)\n",
				detected_rom_size, expected_rom_size);
		}

		printf("Cartridge type: 0x%02X, ROM size code: 0x%02X (%u banks)\n",
			cart_type, rom_size_code, detected_rom_size / 0x4000);
	}


	uint8_t readMemory(uint16_t address)
	{
		if (address == 0xFF00) {
			// Read the Joypad register
			uint8_t joypadSelect = ram[0xFF00] & 0x30; // Bits 4 and 5 (selection)
			uint8_t result = 0xFF;

			if (!(joypadSelect & 0x10)) {
				// Action buttons selected
				result = (result & 0xF0) | (actionState & 0x0F);
			}
			if (!(joypadSelect & 0x20)) {
				// Direction buttons selected
				result = (result & 0xF0) | (directionState & 0x0F);
			}

			// If neither selected, return default (no buttons pressed)
			if (joypadSelect == 0x30) {
				result = 0xFF;
			}

			return result;
		}

		if ((address == 0xFF85))
		{
			//FIX HACK
			//printf("stubbed 0xFF85\n");
			return 0x0F;
		}
		if ((address == 0xFFCC))
		{
			//FIX HACK
			//printf("stubbed 0xFF85\n");
			return 0xFF;
		}
		if ((address == 0xFF01))
		{
			//Serial Stub
			//printf("stubbed 0xFF01\n");
			return 0xFF;
		}

		if (address < 0x4000) {
			// Bank 0 (0x0000 - 0x3FFF) always reads from bank 0
			return full_rom[address];
		}
		else if (address >= 0x4000 && address < 0x8000) {
			// Switchable ROM bank (0x4000 - 0x7FFF)
			uint32_t effective_bank = mbc1_bank1 | (mbc1_bank2 << 5);
			if (mbc1_mode == 1) effective_bank &= 0x1F;
			if (effective_bank == 0) effective_bank = 1;  // Bank 0 is not selectable, maps to bank 1

			uint32_t rom_address = (effective_bank * 0x4000) + (address - 0x4000);
			if (rom_address >= full_rom.size())
			{
				printf("Invalid ROM address: 0x%08X (ROM size: %lu)\n", rom_address, full_rom.size());
				return 0xFF;
			}
			return full_rom[rom_address];
		}
		else if (address >= 0xA000 && address < 0xC000 && mbc1_ram_enable) {
			// External RAM (0xA000 - 0xBFFF)
			uint32_t ram_bank = (mbc1_mode == 1) ? mbc1_bank2 : 0;
			uint32_t ram_address = (ram_bank * 0x2000) + (address - 0xA000);
			if (ram_address >= full_ram.size()) {
				printf("Invalid RAM address: 0x%08X (RAM size: %lu)\n", ram_address, full_ram.size());
				return 0xFF; // Open-bus behavior
			}
			return full_ram[ram_address];
		}

		// Handle other memory regions (WRAM, I/O, etc.)
		return ram[address];
	}

	void writeMemory(uint16_t address, uint8_t value)
	{
		if (address == 0xFF00) {
			// Update bits 4 and 5 (selection) and preserve unused bits as 1
			ram[0xFF00] = (value & 0x30) | 0xCF; // Bits 6-7 and 0-3 default to 1
			return;
		}

		if (address < 0x8000)
		{
			if (address < 0x2000) {
				// Enable/Disable RAM
				mbc1_ram_enable = (value & 0x0A) == 0x0A;
			}
			else if (address >= 0x2000 && address < 0x4000) {
				// ROM Bank Number (5 bits)
				mbc1_bank1 = value & 0x1F;
				if (mbc1_bank1 == 0) mbc1_bank1 = 1;  // Bank 0 is not selectable
				switchROMBank();
			}
			else if (address >= 0x4000 && address < 0x6000) {
				// RAM Bank Number or Upper ROM Bank Bits (2 bits)
				mbc1_bank2 = value & 0x03;
				if (mbc1_mode == 0) {
					switchROMBank();
				}
				else {
					switchRAMBank();
				}
			}
			else if (address >= 0x6000 && address < 0x8000) {
				// ROM/RAM Mode Select
				mbc1_mode = value & 0x01;
			}
			return;
		}

		if (value == 0x39)
		{
			//printf("PC: 0x%04X\n", pc);
		}

		if (address == 0xFF46)
		{
			performDMATransfer(value);
			//ram[address] = value;
		}

		 if (address >= 0xA000 && address < 0xC000 && mbc1_ram_enable) {
			// External RAM (0xA000 - 0xBFFF)
			 uint32_t ram_bank = (mbc1_mode == 1) ? mbc1_bank2 : 0;
			uint32_t ram_address = (ram_bank * 0x2000) + (address - 0xA000);
			full_ram[ram_address] = value;
		 }
		 else
		 {
			 ram[address] = value;
		 }
	}

	void switchROMBank() {
		// Calculate the effective bank number
		uint32_t effective_bank = mbc1_bank1 | (mbc1_bank2 << 5);
		if (effective_bank == 0) effective_bank = 1; // Bank 0 is not selectable, map to bank 1

		uint32_t max_banks = full_rom.size() / 0x4000;
		if (effective_bank >= max_banks)
		{
			printf("Invalid bank number: %u (clamping to %u)\n", effective_bank, max_banks - 1);
			effective_bank = max_banks - 1;
		}

		//printf("Switching to ROM bank: %u\n", effective_bank);
		// Map ROM bank into 0x4000 - 0x7FFF
		// This would actually involve setting up your memory view, or handling it in read/write functions
	}

	void switchRAMBank() {
		// Ensure MBC1 RAM banking mode is selected (mode 1)
		if (mbc1_mode == 1 && mbc1_ram_enable) {
			printf("Switching to RAM bank: %u\n", mbc1_bank2);
			// Map RAM bank into 0xA000 - 0xBFFF
			// This would be handling it in read/write functions
		}
	}

	void writePixels()
	{
		uint8_t lcdc = ram[0xFF40];
		uint8_t currentScanline = ram[0xFF44];
		uint8_t yScroll = ram[0xFF42];
		uint8_t xScroll = ram[0xFF43];
		uint8_t wy = ram[0xFF4A];
		uint8_t wx = ram[0xFF4B];
		wx = (wx < 7) ? 0 : wx - 7;

		if (!(lcdc & (1 << 7))) return;

		uint16_t TILE_DATA_START = (lcdc & (1 << 4)) ? 0x8000 : 0x9000;
		uint16_t BG_TILE_MAP_START = (lcdc & (1 << 3)) ? 0x9C00 : 0x9800;
		uint16_t WIN_TILE_MAP_START = (lcdc & (1 << 6)) ? 0x9800 : 0x9C00;

		const uint16_t TILE_SIZE = 16;
		const int TILE_MAP_WIDTH = 32;
		
		bool windowEnabled = (lcdc & (1 << 5)) && (currentScanline >= wy);
		bool backgroundEnabled = lcdc & (1 << 0);

		const int MODE_2_CYCLES = 80;
		const int MODE_3_CYCLES_MIN = 172;
		const int MODE_3_CYCLES_MAX = 289;
		const int SCANLINE_CYCLES = 456;

		int currentCycle = m_cycles % SCANLINE_CYCLES;

		if (currentCycle >= MODE_2_CYCLES && currentCycle <= (MODE_2_CYCLES + MODE_3_CYCLES_MAX)) {
			//return;
		}

		if (currentScanline >= 144) return;

		for (int screenX = 0; screenX < 160; screenX++)
		{
			uint16_t tileMapAddr;
			uint16_t tileAddr;
			int pixelX, pixelY;
			uint8_t tileIndex;
			uint8_t colorId;

			//printf("WinEnable: %d\n", ram[0xFF40] & (1 << 5));

			if (windowEnabled && screenX >= wx)
			{
				//Window rendering

				//printf("Window rendering\n");

				int windowX = screenX - wx;
				int windowY = currentScanline - wy;

				uint16_t tileX = windowX / 8;
				uint16_t tileY = windowY / 8;

				tileX %= TILE_MAP_WIDTH;
				tileY %= TILE_MAP_WIDTH;

				tileMapAddr = WIN_TILE_MAP_START + tileY * TILE_MAP_WIDTH + tileX;
			}
			else if (backgroundEnabled)
			{
				// Background rendering
				int bgX = (screenX + xScroll) % 256;
				int bgY = (currentScanline + yScroll) % 256;

				uint16_t tileX = bgX / 8;
				uint16_t tileY = bgY / 8;

				tileMapAddr = BG_TILE_MAP_START + tileY * TILE_MAP_WIDTH + tileX;
			}

			renderSprites();

			tileIndex = ram[tileMapAddr];

			if (!(lcdc & (1 << 4))) // If using signed tile indices
			{
				tileAddr = TILE_DATA_START + ((int8_t)tileIndex * TILE_SIZE);
			}
			else
			{
				tileAddr = TILE_DATA_START + (tileIndex * TILE_SIZE);
			}

			pixelX = screenX % 8;
			pixelY = currentScanline % 8;

			uint8_t byte1 = ram[tileAddr + 2 * pixelY];
			uint8_t byte2 = ram[tileAddr + 2 * pixelY + 1];

			uint8_t bitP = 7 - pixelX;
			uint8_t bitOne = (byte1 >> bitP) & 1;
			uint8_t bitTwo = (byte2 >> bitP) & 1;
			colorId = (bitTwo << 1) | bitOne;

			// Calculate the index in the framebuffer
			int index = (143 - currentScanline) * 160 + screenX;

			// Set the pixel in the framebuffer
			ppu.framebuffer[index] = getPalleteColor(colorId);
		}

		last_m_cycles = m_cycles;
	}

	/*void writePixels()
	{
		uint16_t TILE_DATA_START, TILE_DATA_END;
		uint16_t TILE_MAP_START, TILE_MAP_END;
		uint8_t lcdc = ram[0xFF40];

		bool tileDataSigned = !(lcdc & (1 << 4));
		if (tileDataSigned)
		{
			TILE_DATA_START = 0x8800;
			TILE_DATA_END = 0x97FF;
		}
		else
		{
			TILE_DATA_START = 0x8000;
			TILE_DATA_END = 0x8FFF;
		}

		//printf("Tile Data Range: %04X - %04X\n", TILE_DATA_START, TILE_DATA_END);

		if (lcdc & (1 << 3))
		{
			TILE_MAP_START = 0x9C00;
			TILE_MAP_END = 0x9FFF;
		}
		else
		{
			TILE_MAP_START = 0x9800;
			TILE_MAP_END = 0x9BFF;
		}

		const uint16_t TILE_SIZE = 16; // 8x8 tiles, each 16 bytes
		const int TILE_MAP_WIDTH = 32;
		uint8_t yScroll = ram[0xFF42];
		uint8_t xScroll = ram[0xFF43];
		uint8_t currentScanline = ram[0xFF44];  // Current scanline (LY)

		uint8_t lyCompare = ram[0xFF45];
		if (currentScanline == lyCompare)
		{
			//std::cout << "LYC interrupt triggered: LY = " << (int)currentScanline << std::endl;
			ram[0xFF41] |= 0x04;
			if (ram[0xFF41] & 0x40)
			{
				ram[0xFF0F] |= 0x02;
			}
		}
		else
		{
			ram[0xFF41] &= ~0x04;
		}

		uint8_t wy = ram[0xFF4A];
		uint8_t wx = ram[0xFF4B];

		wx = (wx < 7) ? 0 : wx - 7;

		const int MODE_2_CYCLES = 80;
		const int MODE_3_CYCLES_MIN = 172;
		const int MODE_3_CYCLES_MAX = 289;
		const int SCANLINE_CYCLES = 456;

		int currentCycle = m_cycles % SCANLINE_CYCLES;

		if (currentCycle >= MODE_2_CYCLES && currentCycle <= (MODE_2_CYCLES + MODE_3_CYCLES_MAX)) {
			return;
		}

		if (ram[0xFF44] >= 144) ram[0xFF0F] |= 0x01;

		if (currentScanline >= 144) return;

		int screenY = currentScanline;

		uint16_t WINDOW_TILE_MAP_START = (lcdc & (1 << 6)) ? 0x9C00 : 0x9800;
		bool windowEnabled = (lcdc & (1 << 5)) && (currentScanline >= wy);

		

		// Iterate over each pixel on the current scanline
		for (int screenX = 0; screenX < 160; screenX++)
		{
			uint16_t mapAddr;
			uint16_t tileAddr;
			uint8_t tileIndex;
			int pixelX;
			int pixelY;
			uint8_t combined;

			//printf("winenable:%d | ", windowEnabled);
			//printf("screenx >= wx [%d >= %X == %d]\n", screenX, wx, screenX >= wx);

			if (windowEnabled && screenX >= wx)
			{
				//printf("window enable");
				int windowX = screenX - wx;
				int windowY = currentScanline - wy;

				uint16_t tileX = windowX / 8;
				uint16_t tileY = windowY / 8;

				tileX %= TILE_MAP_WIDTH;
				tileY %= TILE_MAP_WIDTH;

				// Calculate tile map address for window
				mapAddr = WINDOW_TILE_MAP_START + tileY * TILE_MAP_WIDTH + tileX;
				tileIndex = ram[mapAddr];

				if (tileDataSigned)
				{
					int8_t signedTileIndex = static_cast<int8_t>(tileIndex);
					tileAddr = TILE_DATA_START + (signedTileIndex * TILE_SIZE);
				}
				else
				{
					tileAddr = TILE_DATA_START + (tileIndex * TILE_SIZE);
				}

				// Get the pixel position within the tile
				pixelX = windowX % 8;
				pixelY = windowY % 8;
			}
			else
			{
				// Calculate the corresponding position in the tile map
				uint16_t tileX = (screenX + xScroll) / 8;
				uint16_t tileY = (screenY + yScroll) / 8;

				// Wrap around the tile map
				tileX %= TILE_MAP_WIDTH;
				tileY %= TILE_MAP_WIDTH;

				//printf("Scroll X: %d, Scroll Y: %d, TileX: %d, TileY: %d\n", xScroll, yScroll, tileX, tileY);

				// Calculate tile map address
				mapAddr = TILE_MAP_START + tileY * TILE_MAP_WIDTH + tileX;
				tileIndex = ram[mapAddr];

				if (tileDataSigned)
				{
					int8_t signedTileIndex = static_cast<int8_t>(tileIndex);
					tileAddr = TILE_DATA_START + (signedTileIndex * TILE_SIZE);
				}
				else
				{
					tileAddr = TILE_DATA_START + (tileIndex * TILE_SIZE);
				}

				// Get the pixel position within the tile
				pixelX = (screenX + xScroll) % 8;
				pixelY = (screenY + yScroll) % 8;
			}

			// Get the relevant bytes from the tile data
			uint8_t byte1 = ram[tileAddr + (pixelY * 2)];
			uint8_t byte2 = ram[tileAddr + (pixelY * 2) + 1];

			// Extract the color value for the pixel
			uint8_t bitP = 7 - pixelX;
			uint8_t bitOne = (byte1 >> bitP) & 1;
			uint8_t bitTwo = (byte2 >> bitP) & 1;
			combined = (bitOne) | (bitTwo << 1);

			// Calculate the index in the framebuffer
			//int index = screenY * 160 + screenX;
			int index = (143 - screenY) * 160 + screenX;

			// Set the pixel in the framebuffer
			ppu.framebuffer[index] = getPalleteColor(combined);
		}

		last_m_cycles = m_cycles;
	}*/

	uint8_t getPalleteColor(uint8_t colorIndex)
	{
		uint8_t palette = ram[0xFF47];  // BGP register

		// Extract color for the given index
		uint8_t shift = colorIndex * 2;
		uint8_t color = (palette >> shift) & 0x03;

		const uint8_t colors[4] = {
			0xFF,  // White
			0xC0,  // Light Gray
			0x80,  // Dark Gray
			0x00   // Black
		};

		return colors[color];
	}



private:
	

	
};
