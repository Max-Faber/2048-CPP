#include <GameState.h>

std::mt19937* GameState::gen;
GameState *GameState::gameState;
FieldPos *GameState::spawnedTile = nullptr;
int GameState::fieldPosCreated = 0, GameState::fieldPosDestroyed = 0;

GameState::GameState(GameState *gameState)
{
    for (int x = 0; x < GameState::gridDimension; x++)
    {
        for (int y = 0; y < GameState::gridDimension; y++)
        {
            FieldPos *fPosCopy = new FieldPos(gameState->fieldTileColumns[x][y]);

            fieldTileColumns[x][y] = fPosCopy;
            fieldTileColumnsReversed[x][gridDimension - y - 1] = fPosCopy;
            fieldTileRows[y][x] = fPosCopy;
            fieldTileRowsReversed[y][gridDimension - x - 1] = fPosCopy;
            if (!fPosCopy->tile) emptyFieldPositions.insert(fPosCopy);
        }
    }
//    for (const std::pair<const int, std::map<const int, TransitionInfo*>>& tInfoMap : gameState->transitions)
//    {
//        for (std::pair<const int, TransitionInfo *> tInfo: tInfoMap.second)
//        {
//            if (!tInfo.second) continue;
//            transitions[tInfoMap.first][tInfo.first] = new TransitionInfo(tInfo.second);
//        }
//    }
    // latestTile = new Tile(gameState->latestTile);
    score = gameState->score;
}

void GameState::initialize()
{
    initializeRandom();
    initializeTileFields();
    initializeGame();
}

void GameState::reset()
{
    for (const std::pair<const int, std::map<const int, FieldPos*>>& fieldTileRow : fieldTileColumns)
    {
        for (std::pair<const int, FieldPos *> fieldTile: fieldTileRow.second)
        {
            if (!fieldTile.second->tile) continue;
            delete fieldTile.second->tile;
            fieldTile.second->tile = nullptr;
            emptyFieldPositions.insert(fieldTile.second);
        }
    }
    cleanTransitionInfo();
    initializeGame();
    score = 0;
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
        FieldPos *pos = new FieldPos(x, y);

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

std::pair<bool, int> GameState::makeMove(Move move, bool trackTransitions)
{
    std::pair<bool, int> mergeResult;

    switch (move)
    {
        case up:
            mergeResult = mergeTiles(GameState::fieldTileRows, trackTransitions);
            break;
        case right:
            mergeResult = mergeTiles(GameState::fieldTileColumnsReversed, trackTransitions);
            break;
        case down:
            mergeResult = mergeTiles(GameState::fieldTileRowsReversed, trackTransitions);
            break;
        case left:
            mergeResult = mergeTiles(GameState::fieldTileColumns, trackTransitions);
            break;
        default:
            break;
    }
    return mergeResult;
}

std::pair<bool, int> GameState::mergeTiles(const std::map<const int, std::map<const int, FieldPos*>>& fieldTilesTwoDim, bool trackTransitions)
{
    int mergeSum = 0;
    bool tilesMoved = false;

    if (gridDimension < 2) return std::pair<bool, int>(tilesMoved, mergeSum);
    for (std::pair<const int, std::map<const int, FieldPos*>> fieldTilesOneDim : fieldTilesTwoDim)
    {
        std::pair<bool, int> mergeResult = mergeTileMap(fieldTilesOneDim.second, trackTransitions);

        tilesMoved |= mergeResult.first;
        mergeSum += mergeResult.second;
        tilesMoved |= fillTileGaps(fieldTilesOneDim.second, trackTransitions);
    }
    return std::pair<bool, int>(tilesMoved, mergeSum);
}

std::pair<bool, int> GameState::mergeTileMap(std::map<const int, FieldPos*>& fieldTilesMap, bool trackTransitions)
{
    std::map<const int, FieldPos*>::iterator itPosLeft, itPosRight;
    std::map<const int, FieldPos*>::iterator fieldTilesMapEnd;
    bool tilesMerged = false;
    int mergeSum = 0;

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
            score += posLeft->tile->val;
            delete posRight->tile;
            posRight->tile = nullptr;
            if (trackTransitions && transitions[posLeft->x][posLeft->y])
            {
                // printf("Leak mergeTileMap!\n");
                delete transitions[posLeft->x][posLeft->y];
            }
            if (trackTransitions) transitions[posLeft->x][posLeft->y] = new TransitionInfo(new FieldPos(posRight), new FieldPos(posLeft));
//            transitions[posLeft->x][posLeft->y] = new TransitionInfo(new FieldPos(posRight), new FieldPos(posLeft));
            emptyFieldPositions.insert(posRight);
            tilesMerged |= true;
            mergeSum += posLeft->tile->val;
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
    return std::pair<bool, int>(tilesMerged, mergeSum);
}

bool GameState::fillTileGaps(std::map<const int, FieldPos*>& fieldTilesMap, bool trackTransitions)
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
        if (trackTransitions && transitions[curEmptyPosition->x][curEmptyPosition->y])
        {
            // printf("Leak fillTileGaps()!\n");
            delete transitions[curEmptyPosition->x][curEmptyPosition->y];
        }
        if (trackTransitions)
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

std::vector<int> GameState::getStateFlattened()
{
    std::vector<int> stateFlattened;

    for (const std::pair<const int, std::map<const int, FieldPos*>>& fieldTileRow : fieldTileColumns)
    {
        for (std::pair<const int, FieldPos*> fieldTile : fieldTileRow.second)
        {
            int val = fieldTile.second->tile ? fieldTile.second->tile->val : 0;

            stateFlattened.push_back(val);
        }
    }
    return stateFlattened;
}

bool GameState::isTerminal()
{
    if (emptyFieldPositions.size() > 0) return false;
    for (int row = 0; row < gridDimension; row++)
    {
        for (int column = 0; column < gridDimension; column++)
        {
            if (!fieldTileRows[row][column]->tile) return false;
        }
    }
    for (int row = 0; row < gridDimension; row++)
    {
        for (int column = 0; column < gridDimension; column++)
        {
            if (row > 0 && fieldTileRows[row][column]->tile->val == fieldTileRows[row - 1][column]->tile->val)
                return false;
            if (column > 0 && fieldTileRows[row][column]->tile->val == fieldTileRows[row][column - 1]->tile->val)
                return false;
            if (row < gridDimension - 1 && fieldTileRows[row][column]->tile->val == fieldTileRows[row + 1][column]->tile->val)
                return false;
            if (column < gridDimension - 1 && fieldTileRows[row][column]->tile->val == fieldTileRows[row][column + 1]->tile->val)
                return false;
        }
    }
    return true;
}

int GameState::getTileValue(int row, int column)
{
    if (!fieldTileRows[row][column]->tile) return 0;
    return fieldTileRows[row][column]->tile->val;
}

TransitionInfo *GameState::getTransitionInfo(int x, int y)
{
    std::map<const int, std::map<const int, TransitionInfo*>>::iterator tInfoX;
    std::map<const int, TransitionInfo*>::iterator tInfoY;

    if ((tInfoX = transitions.find(x)) == transitions.end()) return nullptr;
    if ((tInfoY = tInfoX->second.find(y)) == tInfoX->second.end()) return nullptr;
    return tInfoY->second;
}

void GameState::cleanTransitionInfo()
{
    for (const std::pair<const int, std::map<const int, TransitionInfo*>>& tInfoMap : transitions)
    {
        for (std::pair<const int, TransitionInfo*> tInfo : tInfoMap.second)
        {
            if (!tInfo.second) continue;
            delete tInfo.second;
            tInfo.second = nullptr;
        }
        transitions[tInfoMap.first].clear();
    }
    transitions.clear();
}
