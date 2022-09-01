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
    static std::mt19937* gen; // Standard mersenne_twister_engine seeded with rd()
    static std::set<FieldPos*> emptyFieldPositions;
    const static std::map<std::tuple<int, int>, FieldPos*> fieldTiles;
    const static int initialTileCnt = 2;

    static void initializeRandom();
    static void initializeTileFields();
    static void initializeGame();
    static void spawnTileRandom();
public:
    constexpr static const int gridDimension = 4;

    static void initialize();
};
