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

    explicit Tile(int val) { this->val = val; }
    explicit Tile(Tile *tile)
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
    static std::mt19937 *gen; // Standard mersenne_twister_engine seeded with random_device
    constexpr const static int initialTileCnt = 2;

    static void initializeRandom();
    void initializeTileFields();
    void initializeGame();
    std::pair<bool, int> mergeTileMap(std::map<const int, FieldPos*>& fieldTilesMap, bool trackTransitions);
    bool fillTileGaps(std::map<const int, FieldPos*>& fieldTilesMap, bool trackTransitions);
public:
    static int fieldPosCreated, fieldPosDestroyed;
    constexpr static const int gridDimension = 4;
    int score = 0;
    constexpr static const Move moves[] = { up, down, left, right };
    std::set<FieldPos*> emptyFieldPositions;
    static GameState *gameState;
    // Key of map is the index of the corresponding column (zero-based)
    // Key of the nested map is the index of the corresponding row (zero-based)
    std::map<const int, std::map<const int, FieldPos*>> fieldTileColumns;
    std::map<const int, std::map<const int, FieldPos*>> fieldTileColumnsReversed;
    // Key of map is the index of the corresponding row (zero-based)
    // Key of the nested map is the index of the corresponding column (zero-based)
    std::map<const int, std::map<const int, FieldPos*>> fieldTileRows;
    std::map<const int, std::map<const int, FieldPos*>> fieldTileRowsReversed;

    std::map<const int, std::map<const int, TransitionInfo*>> transitions;

    GameState() = default;
    GameState(GameState *gameState);
    ~GameState()
    {
        for (int x = 0; x < gridDimension; x++)
        {
            for (int y = 0; y < gridDimension; y++)
            {
                delete fieldTileColumns[x][y];
                // delete transitions[x][y];
            }
        }
        // cleanTransitionInfo();
        // delete latestTile; latestTile = nullptr;
    }
    void initialize();
    void reset();
    void printGrid();
    std::pair<bool, int> makeMove(Move move, bool trackTransitions);
    std::pair<bool, int> mergeTiles(const std::map<const int, std::map<const int, FieldPos*>>& fieldTilesTwoDim, bool trackTransitions);
    void spawnTileRandomTest(int val);
    FieldPos *spawnTileRandom();
    std::vector<int> getStateFlattened();
    bool isTerminal();
    int getTileValue(int row, int column);
    TransitionInfo *getTransitionInfo(int x, int y);
    void cleanTransitionInfo();
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

    explicit FieldPos(FieldPos *fPos)
    {
        this->x     = fPos->x;
        this->y     = fPos->y;
        this->tile  = fPos->tile ? new Tile(fPos->tile) : nullptr;
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
};
