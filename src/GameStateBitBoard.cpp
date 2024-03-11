#include <GameStateBitBoard.h>

Board GameStateBitBoard::tableLeft[MAX_ROW_VALUE];
Board GameStateBitBoard::tableRight[MAX_ROW_VALUE];
Board GameStateBitBoard::tableUp[MAX_ROW_VALUE];
Board GameStateBitBoard::tableDown[MAX_ROW_VALUE];
float GameStateBitBoard::rewardTable[MAX_ROW_VALUE];
float GameStateBitBoard::scoreTable[MAX_ROW_VALUE];
int GameStateBitBoard::leftShiftTranspose[] = { 0, 4, 8, 12 };
int GameStateBitBoard::leftShift[] = { 0, 16, 32, 48 };
Board GameStateBitBoard::board = board = 0x0000ULL;
std::unordered_map<Board, transpositionTableEntry> GameStateBitBoard::transpositionTable;

GameStateBitBoard::GameStateBitBoard()
{
    Row resultMoveLeft, resultMoveLeftReverse, rowReverse;

    for (Row row = 0; row < MAX_ROW_VALUE; row++)
    {
        float score = 0.f;
        Tile tiles[4] = { 
            (Tile)((row >>  0) & 0xf),
            (Tile)((row >>  4) & 0xf),
            (Tile)((row >>  8) & 0xf),
            (Tile)((row >> 12) & 0xf)
        };

        for (int i = 0; i < GameState::nMoves; i++)
        {
            int tile = tiles[i];
            
            if (tile < 2) continue;    
            score += (tile - 1) * (1 << tile); // the score is the total sum of the tile and all intermediate merged tiles
        }
        scoreTable[row] = score;

        rewardTable[row] = calculateReward(tiles);
        for (int i = 0, j; i < GameState::gridDimension - 1; i++)
        {
            for (j = i + 1; j < GameState::gridDimension; j++) if (tiles[j] != 0) break; // Found a non-empty tile
            if (j == GameState::gridDimension) break; // Empty row
            if (tiles[i] == 0)
            {   // Move the tile to the empty position
                tiles[i] = tiles[j];
                tiles[j] = 0;
                i--; // It might be the case that more needs to be done so check again
            }
            else if (tiles[i] == tiles[j])
            {
                if (tiles[i] != (Tile)0xf) tiles[i]++;
                tiles[j] = (Tile)0;
            }
        }
        resultMoveLeft = (tiles[0] <<  0) |
                         (tiles[1] <<  4) |
                         (tiles[2] <<  8) |
                         (tiles[3] << 12);
        resultMoveLeftReverse   = reverseRow(resultMoveLeft);
        rowReverse              = reverseRow(row);
        tableLeft[row]          = row                       ^ resultMoveLeft;
        tableRight[rowReverse]  = rowReverse                ^ resultMoveLeftReverse;
        tableUp[row]            = unpackColumn(row)         ^ unpackColumn(resultMoveLeft);
        tableDown[rowReverse]   = unpackColumn(rowReverse)  ^ unpackColumn(resultMoveLeftReverse);
    }
}

float GameStateBitBoard::calculateReward(Tile tiles[])
{
    float sumPower = 0.f, monotonicityLeft = 0.f, monotonicityRight = 0.f;
    int counter = 0, emptyTiles = 0, mergeableTiles = 0;
    Tile currentTile, previousTile = 0;

    for (Move move : GameState::moves)
    {
        currentTile = tiles[move];
        sumPower += pow(currentTile, SUM_POWER);
        if (currentTile == 0) { emptyTiles++; continue; }
        if (previousTile == currentTile) counter++; // Found two equal tiles
        else if (counter > 0)
        {   // Sequence of equal tiles has broken
            mergeableTiles += counter + 1;
            counter = 0;
        }
        previousTile = currentTile;
    }
    if (counter > 0) mergeableTiles += counter + 1; // It's also possible for the sequence of equal tiles to break here (last tile equals second last tile)
    for (int i = 1; i < sizeof(GameState::moves) / sizeof(Move); i++)
    {
        Move move = GameState::moves[i];
        
        currentTile = tiles[i];
        previousTile = tiles[i - 1];
        if (previousTile > currentTile) monotonicityLeft += pow(previousTile, MONOTONICITY_POWER) - pow(currentTile, MONOTONICITY_POWER);
        else monotonicityRight += pow(currentTile, MONOTONICITY_POWER) - pow(previousTile, MONOTONICITY_POWER);
    }
    return 200000.0f +
           EMPTY_WEIGHT        * emptyTiles +
           MERGE_WEIGHT        * mergeableTiles -
           MONOTONICITY_WEIGHT * std::min(monotonicityLeft, monotonicityRight) -
           SUM_WEIGHT          * sumPower;
}

Row GameStateBitBoard::reverseRow(Row row)
{
    return (row >> 12) | ((row >> 4) & 0x00F0) | ((row << 4) & 0x0f00) | (row << 12);
}

Board GameStateBitBoard::unpackColumn(Board row)
{
    return (row | (row << 12ULL) | (row << 24ULL) | (row << 36ULL)) & colMask;
}

Board GameStateBitBoard::transpose(Board board)
{
    Board a1 = board & 0xF0F00F0FF0F00F0FULL;
    Board a2 = board & 0x0000F0F00000F0F0ULL;
    Board a3 = board & 0x0F0F00000F0F0000ULL;
    Board a = a1 | (a2 << 12) | (a3 >> 12);
    Board b1 = a & 0xFF00FF0000FF00FFULL;
    Board b2 = a & 0x00FF00FF00000000ULL;
    Board b3 = a & 0x00000000FF00FF00ULL;

    return b1 | (b2 >> 24) | (b3 << 24);
}

Board GameStateBitBoard::executeMove(Board board, Board boardTranspose, Board table[], int leftShiftValues[])
{
    board ^= table[(boardTranspose >>  0) & rowMask] << leftShiftValues[0];
    board ^= table[(boardTranspose >> 16) & rowMask] << leftShiftValues[1];
    board ^= table[(boardTranspose >> 32) & rowMask] << leftShiftValues[2];
    board ^= table[(boardTranspose >> 48) & rowMask] << leftShiftValues[3];
    return board;
}

Board GameStateBitBoard::makeMove(Board boardBefore, Move move)
{
    Board boardAfter;

    switch (move)
    {
        case up:
            boardAfter = executeMove(boardBefore, transpose(boardBefore), tableUp, leftShiftTranspose);
            break;
        case right:
            boardAfter = executeMove(boardBefore, boardBefore, tableRight, leftShift);
            break;
        case down:
            boardAfter = executeMove(boardBefore, transpose(boardBefore), tableDown, leftShiftTranspose);
            break;
        case left:
            boardAfter = executeMove(boardBefore, boardBefore, tableLeft, leftShift);
            break;
        default:
            break;
    }
    return boardAfter;
}

bool GameStateBitBoard::isTerminal(Board board, std::map<Move, std::pair<Board, bool>> *boardPerMove, std::vector<Move> *validMoves)
{
    bool isTerminal = true;

    for (Move move : GameState::moves)
    {
        Board b = makeMove(board, move);
        bool tilesMoved = b != board;

        if (boardPerMove) (*boardPerMove)[move] = std::pair<Board, bool>(b, tilesMoved);
        if (validMoves && tilesMoved) (*validMoves).push_back(move);
        isTerminal &= !tilesMoved;
    }
    return isTerminal;
}

float GameStateBitBoard::rewardHelper(Board board, const float *table)
{
    return table[(board >> 0)  & rowMask] +
           table[(board >> 16) & rowMask] +
           table[(board >> 32) & rowMask] +
           table[(board >> 48) & rowMask];
}

float GameStateBitBoard::getReward(Board board)
{
    return rewardHelper(board, rewardTable) + rewardHelper(transpose(board), rewardTable);
}

float GameStateBitBoard::getScore(Board board)
{
    return rewardHelper(board, scoreTable);
}

void GameStateBitBoard::reset()
{
    board = 0;
    for (int column = 0; column < GameState::gridDimension; column++)
    {
        for (int row = 0; row < GameState::gridDimension; row++)
        {
            int val = GameState::gameState->getTileValue(row, column);

            if (val == 0) continue;
            insertTile(row, column, val);
        }
    }
}

void GameStateBitBoard::insertTile(int row, int column, int val)
{
    int position = (column * 4) + row;
    Board idx = log2(val);

    idx <<= 4 * position;
    board |= idx;
}

Board GameStateBitBoard::insertTileRandom(Board board)
{
    std::uniform_real_distribution<> distTileVal = std::uniform_real_distribution<>(0, 1);
    std::uniform_int_distribution<> distEmpty = std::uniform_int_distribution<>(0, countEmpty(board) - 1);
    Board tile = distTileVal(*GameState::gen) < 0.9 ? 1 : 2;
    int emptyTileIndex = distEmpty(*GameState::gen);
    Board tempBoard = board;

    while (true)
    {
        while ((tempBoard & 0xf) != 0)
        {
            tempBoard >>= 4;
            tile <<= 4;
        }
        if (emptyTileIndex == 0) break;
        emptyTileIndex--;
        tempBoard >>= 4;
        tile <<= 4;
    }
    if (countEmpty(board) != countEmpty(board | tile) + 1)
    {
        printGrid(board);
        printGrid(board | tile);
        printf("Empty equal!\n");
    }
    return board | tile;
}

int GameStateBitBoard::countEmpty(Board board)
{
    board |= (board >> 2) & 0x3333333333333333ULL;
    board |= (board >> 1);
    board = ~board & 0x1111111111111111ULL;
    board += board >> 32;
    board += board >> 16;
    board += board >>  8;
    board += board >>  4;
    return board & 0xf;
}

void GameStateBitBoard::printGrid(Board board)
{
    Board b = board == 0 ? GameStateBitBoard::board : board;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            uint8_t powerVal = b & 0xf;
            printf("%6u", (powerVal == 0) ? 0 : 1 << powerVal);
            b >>= 4;
        }
        printf("\n");
    }
    printf("\n");
}

int GameStateBitBoard::countDistinctTiles(Board board)
{
    uint16_t bitset = 0;
    int tileCount = 0;

    while (board)
    {
        bitset |= 1 << (board & 0xf);
        board >>= 4;
    }
    bitset >>= 1; // Don't count empty tiles.
    while (bitset)
    {
        bitset &= bitset - 1;
        tileCount++;
    }
    return tileCount;
}

Board GameStateBitBoard::reverseRows(Board b)
{
    uint8_t nRows = sizeof(b) / 2, nBitsRow = sizeof(Row) * 8, nBitsTile = nBitsRow / 4;
    Board reversedRows = 0;

    for (int8_t i = nRows - 1; i >= 0; i--)
    {
        Board row = b >> (i * nBitsRow), reversedRow = 0;

        for (uint8_t j = 0; j < nBitsRow; j += nBitsTile)
        {
            Row tile = (row >> j) & 0xF;

            reversedRow = (reversedRow << nBitsTile) | tile;
        }
        reversedRows = (reversedRows << nBitsRow) | reversedRow;
    }
    return reversedRows;
}

std::unordered_map<Board, transpositionTableEntry>::iterator GameStateBitBoard::searchTranspositionEntry(Board b)
{
    std::unordered_map<Board, transpositionTableEntry>::iterator it = transpositionTable.end();
    uint8_t i = 0;

    while (true)
    {
        it = transpositionTable.find(b);
        if (it != transpositionTable.end()) break;
        if (i == GameState::nMoves - 1) break;
        b = reverseRows(transpose(b));
        i++;
    }
    return it;
}

float GameStateBitBoard::searchRewardTransposition(Board b, uint8_t depth)
{
    std::unordered_map<Board, transpositionTableEntry>::iterator it = searchTranspositionEntry(b);

    // -INFINITY is identification of no entry of the given board in the transposition table being found
    if (it == transpositionTable.end()) return -INFINITY;
    if (it->second.depth < depth) return -INFINITY; // Depth of entry is not deep enough
    return it->second.reward;
}
