#ifndef LEVEL_PARSER_H
#define LEVEL_PARSER_H

#include "Tile.h"

#include <vector>
#include <string>

class LevelParser {
public:
    LevelParser() = default;
    ~LevelParser() = default;

    static std::vector<std::vector<Tile>> parseLevel(std::string filePath);

private:

};

#endif