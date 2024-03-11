// Bit board implementation of the game state of 2048, inspired by https://github.com/nneonneo/2048-ai/tree/master

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>
#include <GameState.h>

#define MAX_ROW_VALUE 0xFFFFULL
#define SUM_POWER 3.5f
#define SUM_WEIGHT 11.0f
#define MONOTONICITY_POWER 4.0f
#define MONOTONICITY_WEIGHT 47.0f
#define EMPTY_WEIGHT 270.f
#define MERGE_WEIGHT 700.f

typedef uint64_t Board;
typedef uint16_t Row;
typedef uint8_t Tile;

struct transpositionTableEntry {
        uint8_t depth;
        float reward;
    };

class GameStateBitBoard
{
private:
    static const Board rowMask = 0xFFFFULL;
    static const Board colMask = 0x000F000F000F000FULL;
    static int leftShift[];
    static int leftShiftTranspose[];
    static Board tableLeft[MAX_ROW_VALUE];
    static Board tableRight[MAX_ROW_VALUE];
    static Board tableUp[MAX_ROW_VALUE];
    static Board tableDown[MAX_ROW_VALUE];
    static float rewardTable[MAX_ROW_VALUE];
    static float scoreTable[MAX_ROW_VALUE];

    static float calculateReward(Tile tiles[]);
    static Row reverseRow(Row row);
    static Board unpackColumn(Board row);
    static Board transpose(Board board);
    static Board executeMove(Board board, Board boardTranspose, Board table[], int leftShiftValues[]);
    static int countEmpty(Board board);
public:
    static Board board;
    static std::unordered_map<Board, transpositionTableEntry> transpositionTable;

    GameStateBitBoard();
    static Board makeMove(Board boardBefore, Move move);
    static bool isTerminal(Board board, std::map<Move, std::pair<Board, bool>> *boardPerMove = nullptr, std::vector<Move> *validMoves = nullptr);
    static float rewardHelper(Board board, const float *table);
    static float getReward(Board board);
    static float getScore(Board board);
    static void reset();
    static void insertTile(int row, int column, int val);
    static Board insertTileRandom(Board board);
    static void printGrid(Board board = 0);
    static int countDistinctTiles(Board board);
    static Board reverseRows(Board b);
    static std::unordered_map<Board, transpositionTableEntry>::iterator searchTranspositionEntry(Board b);
    static float searchRewardTransposition(Board b, uint8_t depth);
};
