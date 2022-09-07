#include <GameLogic.h>

std::mt19937* GameLogic::gen;
std::set<FieldPos*> GameLogic::emptyFieldPositions;
//std::map<std::tuple<int, int>, FieldPos*> GameLogic::fieldTiles;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileColumns;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileColumnsReversed;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileRows;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileRowsReversed;

void GameLogic::initialize() {
    initializeRandom();
    initializeTileFields();
    initializeGame();
}

void GameLogic::initializeRandom() {
    std::random_device rd; // Will be used to obtain a seed for the random number engine

    gen = new std::mt19937(rd());
}

void GameLogic::initializeTileFields() {
    std::vector<int> tileSeq(gridDimension); std::iota(tileSeq.begin(), tileSeq.end(), 0);
    std::vector<std::any> tileSeqAny = CartesianProduct::toUniqueAny(tileSeq);
    std::vector<std::vector<std::any>> tileCoords = CartesianProduct::product({ tileSeqAny, tileSeqAny });

    for (std::vector<std::any> vals : tileCoords) {
        std::map<int, std::map<int, FieldPos*>>::iterator rowIt;
        int x = std::any_cast<int>(vals[0]);
        int y = std::any_cast<int>(vals[1]);
        FieldPos* pos = new FieldPos(x, y);

        fieldTileColumns[x][y] = pos;
        fieldTileColumnsReversed[x][gridDimension - y - 1] = pos;
        fieldTileRows[y][x] = pos;
        fieldTileRowsReversed[y][gridDimension - x - 1] = pos;
        emptyFieldPositions.insert(pos);
        printf("(%d, %d)\n", x, y);
    }
}

void GameLogic::initializeGame() {
    printf("\n");
    for (int _ = 0; _ < initialTileCnt; _++) spawnTileRandom();

//    FieldPos* pos1 = fieldTileRows[0][0];
//    FieldPos* pos2 = fieldTileRows[0][1];
//    FieldPos* pos3 = fieldTileRows[0][2];
//    FieldPos* pos4 = fieldTileRows[0][3];
//
//    pos1->tile = new Tile(2);
//    pos2->tile = new Tile(2);
//    pos3->tile = new Tile(8);
//    pos4->tile = new Tile(2);
//    emptyFieldPositions.erase(pos1);
//    emptyFieldPositions.erase(pos2);
//    emptyFieldPositions.erase(pos3);
//    emptyFieldPositions.erase(pos4);
    printGrid();
}

void GameLogic::spawnTileRandom() {
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

void GameLogic::printGrid() {
    for (const std::pair<const int, std::map<int, FieldPos*>>& fieldTileRow : fieldTileColumns) {
        int x = fieldTileRow.first;

        for (std::pair<const int, FieldPos*> fieldTile : fieldTileRow.second) {
            int y = fieldTile.first;

            if (!fieldTile.second->tile) { printf("#\t"); continue; }
            printf("%d\t", fieldTile.second->tile->val);
        }
        printf("\n\n");
    }
    printf("\n");
}

void GameLogic::mergeTiles(const std::map<int, std::map<int, FieldPos*>>& fieldTilesTwoDim) {
    if (gridDimension < 2) return;
    for (std::pair<const int, std::map<int, FieldPos*>> fieldTilesOneDim : fieldTilesTwoDim) {
        mergeTileMap(fieldTilesOneDim.second);
        fillTileGaps(fieldTilesOneDim.second);
    }
}

void GameLogic::mergeTileMap(std::map<int, FieldPos*>& fieldTilesMap) {
    std::map<int, FieldPos*>::iterator itPosLeft, itPosRight;
    std::map<int, FieldPos*>::iterator fieldTilesMapEnd;

    itPosLeft        = fieldTilesMap.begin();
    itPosRight       = std::next(fieldTilesMap.begin());
    fieldTilesMapEnd = fieldTilesMap.end();
    do {
        FieldPos* posLeft  = itPosLeft->second;
        FieldPos* posRight = itPosRight->second;

        if (posLeft->tile && posRight->tile && posLeft->tile->val == posRight->tile->val) {
            posLeft->tile->val *= 2;
            delete posRight->tile;
            posRight->tile = nullptr;
        }
        itPosLeft++;
        itPosRight++;
    } while (itPosRight != fieldTilesMapEnd);
}

void GameLogic::fillTileGaps(std::map<int, FieldPos*>& fieldTilesMap) {
    FieldPos* curEmptyPosition = nullptr;
    std::vector<FieldPos*> emptyPositions = { };

    for (std::pair<const int, FieldPos*> pos : fieldTilesMap) {
        if (!pos.second->tile) emptyPositions.push_back(pos.second);
        if (!curEmptyPosition) {
            std::vector<FieldPos*>::iterator it = emptyPositions.begin();

            if (it == emptyPositions.end()) continue;
            curEmptyPosition = *it;
            emptyPositions.erase(it);
        }
        if (!pos.second->tile) continue;
        curEmptyPosition->tile = pos.second->tile;
        emptyPositions.push_back(pos.second);
        pos.second->tile = nullptr;
        curEmptyPosition = nullptr;
    }
}
