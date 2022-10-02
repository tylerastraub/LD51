#ifndef LEVEL_H
#define LEVEL_H

#include "Tile.h"
#include "Spritesheet.h"

#include <vector>

class Level {
public:
    Level() = default;
    ~Level() = default;

    void render(int xOffset, int yOffset);

    void setTilemap(std::vector<std::vector<Tile>> tilemap);
    void setTileSize(int tileSize);
    void setTileAt(int x, int y, Tile tile);
    void setTileset(Spritesheet* tileset);

    Tile getTileAt(int x, int y);
    int getTileSize();
    int getTilemapWidth();
    int getTilemapHeight();

private:
    std::vector<std::vector<Tile>> _tilemap;
    int _tilemapWidth = 0;
    int _tilemapHeight = 0;
    int _tileSize = 16;
    Spritesheet* _tileset = nullptr;

};

#endif