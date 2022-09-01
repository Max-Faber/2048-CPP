#include <GameLogic.h>

void GameLogic::initialize() {
    // CartesianProduct::testCartesianProduct();
    initializeRandom();
    initializeTileFields();
    initializeGame();
}

std::mt19937* GameLogic::gen;
void GameLogic::initializeRandom() {
    std::random_device rd; // Will be used to obtain a seed for the random number engine

    gen = new std::mt19937(rd());
}

std::set<FieldPos*> GameLogic::emptyFieldPositions;
void GameLogic::initializeTileFields() {
    std::vector<int> tileSeq(gridDimension); std::iota(tileSeq.begin(), tileSeq.end(), 1);
    std::vector<std::any> tileSeqAny = CartesianProduct::toUniqueAny(tileSeq);
    std::vector<std::vector<std::any>> tileCoords = CartesianProduct::product(std::vector<std::vector<std::any>> {
        tileSeqAny, tileSeqAny
    });

    for (std::vector<std::any> vals : tileCoords) {
        int x = std::any_cast<int>(vals[0]);
        int y = std::any_cast<int>(vals[1]);

        printf("(%d, %d)\n", x, y);
        emptyFieldPositions.insert(new FieldPos(x, y));
    }
}

void GameLogic::initializeGame() {
    printf("\n");
    for (int i = 0; i < initialTileCnt; i++) spawnTileRandom();
}

void GameLogic::spawnTileRandom() {
    FieldPos *pos;
    std::uniform_int_distribution<> distrib(0, (int)emptyFieldPositions.size() - 1);
    std::set<FieldPos*>::iterator randFieldPosIt = emptyFieldPositions.begin();

    std::advance(randFieldPosIt, distrib(*gen));
    pos = *randFieldPosIt;
    emptyFieldPositions.erase(randFieldPosIt);
    printf("(%d, %d)\n", pos->x, pos->y);
}
