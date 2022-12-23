#include <GameLogic.h>

std::mt19937* GameLogic::gen;
std::set<FieldPos*> GameLogic::emptyFieldPositions;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileColumns;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileColumnsReversed;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileRows;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileRowsReversed;

void GameLogic::initialize()
{
    initializeRandom();
    initializeTileFields();
    initializeGame();
}

void GameLogic::initializeRandom()
{
    std::random_device rd; // Will be used to obtain a seed for the random number engine

    gen = new std::mt19937(rd());
}

void GameLogic::initializeTileFields()
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

void GameLogic::initializeGame()
{
    for (int _ = 0; _ < initialTileCnt; _++) spawnTileRandom();
    printGrid();
}

void GameLogic::spawnTileRandom()
{
    FieldPos *pos;
    int tileSetUpperRange                          = (int)emptyFieldPositions.size() - 1;
    std::uniform_int_distribution<> distTileCoords = std::uniform_int_distribution<>(0, tileSetUpperRange);
    std::uniform_int_distribution<> distTileVal    = std::uniform_int_distribution<>(1, 2);
    std::set<FieldPos*>::iterator randFieldPosIt   = emptyFieldPositions.begin();

    std::advance(randFieldPosIt, distTileCoords(*gen));
    pos = *randFieldPosIt;
    pos->tile = new Tile(distTileVal(*gen) * 2);
    emptyFieldPositions.erase(randFieldPosIt);
}

void GameLogic::printGrid()
{
    for (const std::pair<const int, std::map<int, FieldPos*>>& fieldTileRow : fieldTileColumns)
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

bool GameLogic::mergeTiles(const std::map<int, std::map<int, FieldPos*>>& fieldTilesTwoDim)
{
    bool tilesMoved = false;

    if (gridDimension < 2) return tilesMoved;
    for (std::pair<const int, std::map<int, FieldPos*>> fieldTilesOneDim : fieldTilesTwoDim)
    {
        tilesMoved |= mergeTileMap(fieldTilesOneDim.second);
        tilesMoved |= fillTileGaps(fieldTilesOneDim.second);
    }
    return tilesMoved;
}

bool GameLogic::mergeTileMap(std::map<int, FieldPos*>& fieldTilesMap)
{
    std::map<int, FieldPos*>::iterator itPosLeft, itPosRight;
    std::map<int, FieldPos*>::iterator fieldTilesMapEnd;
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
            posLeft->tile->val *= 2;
            delete posRight->tile;
            posRight->tile = nullptr;
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

bool GameLogic::fillTileGaps(std::map<int, FieldPos*>& fieldTilesMap)
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
        curEmptyPosition->tile = pos.second->tile;
        emptyFieldPositions.insert(pos.second);
        emptyFieldPositions.erase(curEmptyPosition);
        emptyPositions.push_back(pos.second);
        pos.second->tile = nullptr;
        curEmptyPosition = nullptr;
    }
    return tilesMoved;
}
