#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <iostream>
#include <bitset>
#include "load_save_png.hpp"
#include "PPU466.hpp"

class AssetPipeline
{

public:
    void loadSpriteSheet(std::string filename);
    void parseLevel(int levelIndex);
    void parseObstacle(int obsIndex);
    void parseSpriteNbyN(int blx, int Nx, int bly, int Ny, int spriteIndx);
    void parseSprite(int x, int y, int spriteIndx);
    void setPPU(PPU466& ppuRef) { ppu = &ppuRef; }

private:
    int returnColourIndex(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    std::vector<glm::u8vec4> spriteSheetData;
    OriginLocation sheetOriginLoc = LowerLeftOrigin;
    glm::uvec2 spriteSheetSize;
    PPU466* ppu = nullptr;
};
