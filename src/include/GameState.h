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

enum Move { up, down, left, right };

struct TransitionInfo;
struct FieldPos;
class GameState
{
private:
    static std::mt19937* gen; // Standard mersenne_twister_engine seeded with random_device

    static std::set<FieldPos*> emptyFieldPositions;

    constexpr const static int initialTileCnt = 2;

    static void initializeRandom();
    static void initializeTileFields();
    static void initializeGame();
    static std::pair<bool, int> mergeTileMap(std::map<const int, FieldPos*>& fieldTilesMap);
    static bool fillTileGaps(std::map<const int, FieldPos*>& fieldTilesMap);
public:
    static int fieldPosCreated, fieldPosDestroyed;
    constexpr static const int gridDimension = 4;
    static int score;
    // Key of map is the index of the corresponding column (zero-based)
    // Key of the nested map is the index of the corresponding row (zero-based)
    static std::map<const int, std::map<const int, FieldPos*>> fieldTileColumns;
    static std::map<const int, std::map<const int, FieldPos*>> fieldTileColumnsReversed;
    // Key of map is the index of the corresponding row (zero-based)
    // Key of the nested map is the index of the corresponding column (zero-based)
    static std::map<const int, std::map<const int, FieldPos*>> fieldTileRows;
    static std::map<const int, std::map<const int, FieldPos*>> fieldTileRowsReversed;

    static std::map<const int, std::map<const int, TransitionInfo*>> transitions;

    static void initialize();
    static void reset();
    static void printGrid();
    static std::pair<bool, int> makeMove(Move move);
    static std::pair<bool, int> mergeTiles(const std::map<const int, std::map<const int, FieldPos*>>& fieldTilesTwoDim);
    static void spawnTileRandomTest(int val);
    static FieldPos* spawnTileRandom();
    static std::vector<int> getStateFlattened();
    static bool isTerminal();
    static int getTileValue(int row, int column);
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
        // printf("GameState::fieldPosCreated: %d\n", GameState::fieldPosCreated += 1);
    }

    explicit FieldPos(FieldPos* fPos)
    {
        this->x     = fPos->x;
        this->y     = fPos->y;
        this->tile  = new Tile(fPos->tile);
        // printf("GameState::fieldPosCreated: %d\n", GameState::fieldPosCreated += 1);
    }

    ~FieldPos()
    {
        if (!tile) return;
        delete tile;
        tile = nullptr;
        // printf("GameState::fieldPosDestroyed: %d\n", GameState::fieldPosDestroyed += 1);
    }
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

    static TransitionInfo* getTransitionInfo(int x, int y)
    {
        std::map<const int, std::map<const int, TransitionInfo*>>::iterator tInfoX;
        std::map<const int, TransitionInfo*>::iterator tInfoY;

        if ((tInfoX = GameState::transitions.find(x)) == GameState::transitions.end()) return nullptr;
        if ((tInfoY = tInfoX->second.find(y)) == tInfoX->second.end()) return nullptr;
        return tInfoY->second;
    }

    static void cleanTransitionInfo()
    {
        for (const std::pair<const int, std::map<const int, TransitionInfo *>>& tInfoMap : GameState::transitions)
        {
            for (std::pair<const int, TransitionInfo*> tInfo : tInfoMap.second)
            {
                if (!tInfo.second) continue;
                delete tInfo.second;
                tInfo.second = nullptr;
            }
            GameState::transitions[tInfoMap.first].clear();
        }
        GameState::transitions.clear();
    }
};


