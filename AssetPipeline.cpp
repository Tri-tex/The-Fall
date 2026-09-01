#include "AssetPipeline.hpp"

void AssetPipeline::loadSpriteSheet(std::string filename)
{
    load_png(filename, &spriteSheetSize, &spriteSheetData, sheetOriginLoc);
}

void AssetPipeline::parseLevel(int levelIndex)
{
    if (ppu == nullptr) { return; }
    int blx = (levelIndex);
    int bly = 14;

    int startTileIndex = 3 + (levelIndex * 4);

    int Nx = 2;
    int Ny = 2;

    parseSpriteNbyN(blx, Nx, bly, Ny, startTileIndex);
}

void AssetPipeline::parseObstacle(int obsIndex)
{
    parseSprite(8, obsIndex, 42 + obsIndex);
}

void AssetPipeline::parseSpriteNbyN(int blx, int Nx, int bly, int Ny, int spriteIndx)
{

    if (ppu == nullptr) { return; }
    if (blx < 0 || blx + Nx > 16 || bly < 0 || bly + Ny > 16) { return; }

    for (int i = 0; i < Ny + 1; i++)
    {
        for (int j = 0; j < Nx + 1; j++)
        {
            int x = blx + j;
            int y = bly + i; 
            std::cout << x << std::endl;
            std::cout << y << std::endl;
            int tileIndex = spriteIndx + (i * Nx + j);

            parseSprite(x, y, tileIndex);
        } 
    }
    
}

void AssetPipeline::parseSprite(int x, int y, int spriteIndx)
{
    //Make sure coordinate is within range
    if (ppu == nullptr) { return; }
    if (x < 0 || x >= 16 || y < 0 || y >= 16) { return; }

    int width = 8;
    int height = 8;
    std::array<uint8_t, 8> rowbit0 = {};
    std::array<uint8_t, 8> rowbit1 = {};

    for (size_t j = y * height; j < (y+1) * height; ++j)
    {
        size_t localRow = j - (y * height);

        for (size_t i = x * width; i < (x + 1) * width; ++i)
        {
            size_t localCol = i - (x * width);
            auto pixel = spriteSheetData[j * spriteSheetSize.x + i];
            int colouridx = returnColourIndex(pixel.r, pixel.g, pixel.b, pixel.a);
            
            uint8_t bit0 = colouridx & 1;
            uint8_t bit1 = (colouridx >> 1) & 1;
            
            rowbit0[localRow] |= (bit0 << localCol);
            rowbit1[localRow] |= (bit1 << localCol);
        }
    }

    if (spriteIndx >= 0)
    {
        ppu->tile_table[spriteIndx].bit0 = rowbit0;
        ppu->tile_table[spriteIndx].bit1 = rowbit1;
    }
}

int AssetPipeline::returnColourIndex(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (a == 0) { return 0; }   
    // formula from https://stackoverflow.com/questions/596216/formula-to-determine-perceived-brightness-of-rgb-color
    // using perceived option 1
    float luminance = 0.299f * r + 0.584f * g + 0.114f * b;

    if (luminance <= 85){ return 1; }
    else if (luminance <= 170){ return 2; }
    else{ return 3; }
}


