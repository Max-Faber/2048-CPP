#pragma once

#include <vector>
#include <cstdio>
#include <numeric>
#include <map>
#include <random>
#include <CartesianProduct.h>

struct Tile
{
    int val = 0;

    explicit Tile(int val)    { this->val = val; }
    explicit Tile(Tile* tile)
    {
        if (tile) this->val = tile->val;
    }
};

struct FieldPos
{
    int x, y;
    Tile *tile;

    FieldPos(int x, int y, Tile *tile = nullptr)
    {
        this->x     = x;
        this->y     = y;
        this->tile  = tile;
    }

    explicit FieldPos(FieldPos* fPos)
    {
        this->x     = fPos->x;
        this->y     = fPos->y;
        this->tile  = new Tile(fPos->tile);
    }

    ~FieldPos()
    {
        if (!tile) return;
        delete tile;
        tile = nullptr;
    }
};

struct TransitionInfo;

class GameState
{
private:
    static std::mt19937* gen; // Standard mersenne_twister_engine seeded with random_device

    static std::set<FieldPos*> emptyFieldPositions;

    constexpr const static int initialTileCnt = 2;

    static void initializeRandom();
    static void initializeTileFields();
    static void initializeGame();
    static bool mergeTileMap(std::map<const int, FieldPos*>& fieldTilesMap);
    static bool fillTileGaps(std::map<const int, FieldPos*>& fieldTilesMap);
public:
    constexpr static const int gridDimension = 4;
    // Key of map is the index of the corresponding column (zero-based)
    // Key of the nested map is the index of the corresponding row (zero-based)
    static std::map<const int, std::map<const int, FieldPos*>> fieldTileColumns;
    static std::map<const int, std::map<const int, FieldPos*>> fieldTileColumnsReversed;
    // Key of map is the index of the corresponding row (zero-based)
    // Key of the nested map is the index of the corresponding column (zero-based)
    static std::map<const int, std::map<const int, FieldPos*>> fieldTileRows;
    static std::map<const int, std::map<const int, FieldPos*>> fieldTileRowsReversed;

    static std::map<const int, std::map<const int, TransitionInfo*>> merges;
    static std::map<const int, std::map<const int, TransitionInfo*>> transitions;

    static void initialize();
    static void printGrid();
    static bool mergeTiles(const std::map<const int, std::map<const int, FieldPos*>>& fieldTilesTwoDim);
    static void spawnTileRandomTest(int val);
    static void spawnTileRandom();
};

struct TransitionInfo
{
    FieldPos* source;
    FieldPos* target;

    TransitionInfo(FieldPos* source, FieldPos* target)
    {
        this->source = source;
        this->target = target;
    }

    explicit TransitionInfo(TransitionInfo* tInfo)
    {
        this->source = new FieldPos(tInfo->source);
        this->target = new FieldPos(tInfo->target);
    }

    ~TransitionInfo()
    {
        if (source) { delete source; source = nullptr; }
        if (target) { delete target; target = nullptr; }
    }

//    explicit TransitionInfo(TransitionInfo* tInfo)
//    {
//        this->source = tInfo->source;
//        this->target = tInfo->target;
//    }

    static TransitionInfo* getTransitionInfo(int x, int y)
    {
        std::map<const int, std::map<const int, TransitionInfo*>>::iterator tInfoX;
        std::map<const int, TransitionInfo*>::iterator tInfoY;

        if ((tInfoX = GameState::transitions.find(x)) == GameState::transitions.end()) return nullptr;
        if ((tInfoY = tInfoX->second.find(y)) == tInfoX->second.end()) return nullptr;
        return tInfoY->second;
    }
};
