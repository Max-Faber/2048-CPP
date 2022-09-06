#include <GameLogic.h>

std::mt19937* GameLogic::gen;
std::set<FieldPos*> GameLogic::emptyFieldPositions;
//std::map<std::tuple<int, int>, FieldPos*> GameLogic::fieldTiles;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileColumns;
std::map<int, std::map<int, FieldPos*>> GameLogic::fieldTileRows;

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
        fieldTileRows[y][x] = pos;
        emptyFieldPositions.insert(pos);
        printf("(%d, %d)\n", x, y);
    }
}

void GameLogic::initializeGame() {
    printf("\n");
    for (int _ = 0; _ < initialTileCnt; _++) spawnTileRandom();

//    FieldPos* pos1 = fieldTileRows[0][0];
//    FieldPos* pos2 = fieldTileRows[1][0];
//    FieldPos* pos3 = fieldTileRows[2][0];
//    FieldPos* pos4 = fieldTileRows[3][0];
//
//    pos1->tile = new Tile(2);
//    pos2->tile = new Tile(2);
//    pos3->tile = new Tile(2);
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

            if (!fieldTile.second->tile) { printf("# "); continue; }
            printf("%d ", fieldTile.second->tile->val);
        }
        printf("\n");
    }
    printf("\n");
}

void GameLogic::mergeTiles(const std::map<int, std::map<int, FieldPos*>>& fieldTilesTwoDim) {
    if (gridDimension < 2) return;
    for (std::pair<const int, std::map<int, FieldPos*>> fieldTilesOneDim : fieldTilesTwoDim) {
        mergeTileMap(fieldTilesOneDim.second);
    }
    printGrid();
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
            for (std::map<int, FieldPos*>::iterator it = itPosRight; it != fieldTilesMapEnd; it++) {
                std::map<int, FieldPos*>::iterator itNext;

                itNext = std::next(it);
                if (itNext == fieldTilesMapEnd) {
                    if (it->second->tile) it->second->tile = nullptr;
                    break;
                }
                it->second->tile = itNext->second->tile;
            }
        }
        itPosLeft++;
        itPosRight++;
    } while (itPosRight != fieldTilesMapEnd);
}
