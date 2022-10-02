#include "LevelParser.h"
#include "FileIO.h"

#include <algorithm>

std::vector<std::vector<Tile>> LevelParser::parseLevel(std::string filePath) {
    Tile tl = {TileType::SOLID, {0, 0, 16, 16}}; // top left
    Tile t = {TileType::SOLID, {1, 0, 16, 16}}; // top
    Tile tr = {TileType::SOLID, {2, 0, 16, 16}}; // top right
    Tile l = {TileType::SOLID, {0, 1, 16, 16}}; // left
    Tile c = {TileType::SOLID, {1, 1, 16, 16}}; // center
    Tile r = {TileType::SOLID, {2, 1, 16, 16}}; // right
    Tile bl = {TileType::SOLID, {0, 2, 16, 16}}; // bot left
    Tile b = {TileType::SOLID, {1, 2, 16, 16}}; // bot
    Tile br = {TileType::SOLID, {2, 2, 16, 16}}; // bot right
    Tile s = {TileType::SOLID, {3, 0, 16, 16}}; // single tile
    Tile v = {TileType::HAZARD, {0, 3, 16, 16}}; // spike tile
    Tile o = {TileType::NOVAL, {0, 0, 0, 0}}; // empty tile

    auto levelContents = FileIO::readFile(filePath);
    std::vector<std::vector<Tile>> result;
    for(auto line : levelContents) {
        int pos = 0;
        std::vector<Tile> row;
        while((pos = line.find(',')) != std::string::npos) {
            std::string token = line.substr(0, pos);
            token.erase(std::remove(token.begin(), token.end(), ' '), token.end());

            if(token == "tl") {
                row.push_back(tl);
            }
            else if(token == "t") {
                row.push_back(t);
            }
            else if(token == "tr") {
                row.push_back(tr);
            }
            else if(token == "l") {
                row.push_back(l);
            }
            else if(token == "c") {
                row.push_back(c);
            }
            else if(token == "r") {
                row.push_back(r);
            }
            else if(token == "bl") {
                row.push_back(bl);
            }
            else if(token == "b") {
                row.push_back(b);
            }
            else if(token == "br") {
                row.push_back(br);
            }
            else if(token == "s") {
                row.push_back(s);
            }
            else if(token == "v") {
                row.push_back(v);
            }
            else {
                row.push_back(o);
            }

            line.erase(0, pos + 1);
        }
        result.push_back(row);
    }

    return result;
}