#pragma once

#include <vector>
#include <cstdio>
#include <numeric>
#include <map>
#include <random>
#include <CartesianProduct.h>
#ifdef __APPLE_CC__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

struct Tile {
    int val;

    Tile(int val) {
        this->val = val;
    }
};

struct FieldPos {
    int x, y;
    Tile *tile;

    FieldPos(int x, int y, Tile *tile = nullptr) {
        this->x     = x;
        this->y     = y;
        this->tile  = tile;
    }

    ~FieldPos() {
        if (!tile) return;
        delete tile;
        tile = nullptr;
    }
};

class GameLogic {
private:
    static std::mt19937* gen; // Standard mersenne_twister_engine seeded with random_device

    static std::set<FieldPos*> emptyFieldPositions;

//    static std::map<std::tuple<int, int>, FieldPos*> fieldTiles;

    constexpr const static int initialTileCnt = 4*4;

    static void initializeRandom();
    static void initializeTileFields();
    static void initializeGame();
    static void spawnTileRandom();
    static void mergeTileMap(std::map<int, FieldPos*>& fieldTilesMap);
public:
    constexpr static const int gridDimension = 4;
    // Key of map is the index of the corresponding column (zero-based)
    // Key of the nested map is the index of the corresponding row (zero-based)
    static std::map<int, std::map<int, FieldPos*>> fieldTileColumns;
    // Key of map is the index of the corresponding row (zero-based)
    // Key of the nested map is the index of the corresponding column (zero-based)
    static std::map<int, std::map<int, FieldPos*>> fieldTileRows;

    static void initialize();
    static void printGrid();
    static void mergeTiles(const std::map<int, std::map<int, FieldPos*>>& fieldTilesTwoDim);
};
