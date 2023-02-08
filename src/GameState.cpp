#include <GameState.h>

std::mt19937* GameState::gen;
std::set<FieldPos*> GameState::emptyFieldPositions;
std::map<const int, std::map<const int, FieldPos*>> GameState::fieldTileColumns;
std::map<const int, std::map<const int, FieldPos*>> GameState::fieldTileColumnsReversed;
std::map<const int, std::map<const int, FieldPos*>> GameState::fieldTileRows;
std::map<const int, std::map<const int, FieldPos*>> GameState::fieldTileRowsReversed;
std::map<const int, std::map<const int, TransitionInfo*>> GameState::transitions;
int GameState::score = 0;
int GameState::fieldPosCreated = 0, GameState::fieldPosDestroyed = 0;

void GameState::initialize()
{
    initializeRandom();
    initializeTileFields();
    initializeGame();
}

void GameState::initializeRandom()
{
    std::random_device rd; // Will be used to obtain a seed for the random number engine

    gen = new std::mt19937(rd());
}

void GameState::initializeTileFields()
{
    std::vector<int> tileSeq(gridDimension); std::iota(tileSeq.begin(), tileSeq.end(), 0);
    std::vector<std::any> tileSeqAny = CartesianProduct::toUniqueAny(tileSeq);
    std::vector<std::vector<std::any>> tileCoords = CartesianProduct::product({ tileSeqAny, tileSeqAny });

    for (std::vector<std::any> vals : tileCoords)
    {
        int x = std::any_cast<int>(vals[0]);
        int y = std::any_cast<int>(vals[1]);
        FieldPos* pos = new FieldPos(x, y);

        fieldTileColumns[x][y] = pos;
        fieldTileColumnsReversed[x][gridDimension - y - 1] = pos;
        fieldTileRows[y][x] = pos;
        fieldTileRowsReversed[y][gridDimension - x - 1] = pos;
        emptyFieldPositions.insert(pos);
    }
}

void GameState::initializeGame()
{
    for (int _ = 0; _ < initialTileCnt; _++) spawnTileRandom();
//    spawnTileRandomTest(512);
//    spawnTileRandomTest(1024);
//    spawnTileRandomTest(2048);
//    spawnTileRandomTest(4096);
//    spawnTileRandomTest(8192);
//    spawnTileRandomTest(16384);
//    spawnTileRandomTest(65536);
//    spawnTileRandomTest(131072);
}

void GameState::spawnTileRandomTest(int val)
{
    FieldPos *pos;
    int tileSetUpperRange                          = (int)emptyFieldPositions.size() - 1;
    std::uniform_int_distribution<> distTileCoords = std::uniform_int_distribution<>(0, tileSetUpperRange);
    std::uniform_int_distribution<> distTileVal    = std::uniform_int_distribution<>(1, 2);
    std::set<FieldPos*>::iterator randFieldPosIt   = emptyFieldPositions.begin();

    std::advance(randFieldPosIt, distTileCoords(*gen));
    pos = *randFieldPosIt;
    pos->tile = new Tile(val);
    emptyFieldPositions.erase(randFieldPosIt);
}

FieldPos* GameState::spawnTileRandom()
{
    FieldPos *pos;
    int tileSetUpperRange                          = (int)emptyFieldPositions.size() - 1;
    std::uniform_int_distribution<> distTileCoords = std::uniform_int_distribution<>(0, tileSetUpperRange);
    std::uniform_real_distribution<> distTileVal   = std::uniform_real_distribution<>(0, 1);
    std::set<FieldPos*>::iterator randFieldPosIt   = emptyFieldPositions.begin();

    std::advance(randFieldPosIt, distTileCoords(*gen));
    pos = *randFieldPosIt;
    pos->tile = new Tile(distTileVal(*gen) < 0.9 ? 2 : 4);
    emptyFieldPositions.erase(randFieldPosIt);
    return pos;
}

void GameState::printGrid()
{
    for (const std::pair<const int, std::map<const int, FieldPos*>>& fieldTileRow : fieldTileColumns)
    {
        for (std::pair<const int, FieldPos*> fieldTile : fieldTileRow.second)
        {
            if (!fieldTile.second->tile) { printf("#\t"); continue; }
            printf("%d\t", fieldTile.second->tile->val);
        }
        printf("\n\n");
    }
    printf("\n");
}

bool GameState::mergeTiles(const std::map<const int, std::map<const int, FieldPos*>>& fieldTilesTwoDim)
{
    bool tilesMoved = false;

    if (gridDimension < 2) return tilesMoved;
    for (std::pair<const int, std::map<const int, FieldPos*>> fieldTilesOneDim : fieldTilesTwoDim)
    {
        tilesMoved |= mergeTileMap(fieldTilesOneDim.second);
        tilesMoved |= fillTileGaps(fieldTilesOneDim.second);
    }
    return tilesMoved;
}

bool GameState::mergeTileMap(std::map<const int, FieldPos*>& fieldTilesMap)
{
    std::map<const int, FieldPos*>::iterator itPosLeft, itPosRight;
    std::map<const int, FieldPos*>::iterator fieldTilesMapEnd;
    bool tilesMerged = false;

    itPosLeft        = fieldTilesMap.begin();
    itPosRight       = std::next(fieldTilesMap.begin());
    fieldTilesMapEnd = fieldTilesMap.end();
    do
    {
        FieldPos* posLeft  = itPosLeft->second;
        FieldPos* posRight = itPosRight->second;

        if (posLeft->tile && posRight->tile && posLeft->tile->val == posRight->tile->val)
        {
            TransitionInfo* tInfo = new TransitionInfo(new FieldPos(posRight), new FieldPos(posLeft));

            posLeft->tile->val *= 2;
            score += posLeft->tile->val;
            printf("Score: %d\n", score);
            delete posRight->tile;
            posRight->tile = nullptr;
            transitions[posLeft->x][posLeft->y] = tInfo;//new TransitionInfo(tInfo);
            emptyFieldPositions.insert(posRight);
            tilesMerged |= true;
        }
        else if (posLeft->tile && !posRight->tile)
        {
            itPosRight++;
            continue;
        }
        else if (itPosLeft->first != itPosRight->first + 1) itPosLeft = std::prev(itPosRight);
        itPosLeft++;
        itPosRight++;
    } while (itPosRight != fieldTilesMapEnd);
    return tilesMerged;
}

bool GameState::fillTileGaps(std::map<const int, FieldPos*>& fieldTilesMap)
{
    bool tilesMoved = false;
    FieldPos* curEmptyPosition = nullptr;
    std::vector<FieldPos*> emptyPositions = { };

    for (std::pair<const int, FieldPos*> pos : fieldTilesMap)
    {
        if (!pos.second->tile) emptyPositions.push_back(pos.second);
        if (!curEmptyPosition)
        {
            std::vector<FieldPos*>::iterator it = emptyPositions.begin();

            if (it == emptyPositions.end()) continue;
            curEmptyPosition = *it;
            emptyFieldPositions.insert(curEmptyPosition);
            emptyPositions.erase(it);
        }
        if (!pos.second->tile) continue;
        tilesMoved = true;
        // pos.second == from
        // curEmptyPosition == to
        transitions[curEmptyPosition->x][curEmptyPosition->y] = new TransitionInfo(
                new FieldPos(pos.second),
                new FieldPos(curEmptyPosition)
        );
        curEmptyPosition->tile = pos.second->tile;
        emptyFieldPositions.insert(pos.second);
        emptyFieldPositions.erase(curEmptyPosition);
        emptyPositions.push_back(pos.second);
        pos.second->tile = nullptr;
        curEmptyPosition = nullptr;
    }
    return tilesMoved;
}
