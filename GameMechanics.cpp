#include <GameMechanics.h>

GameMechanics::GameMechanics() {

};


void GameMechanics::initializeTiles() {
    for (int row = 0; row < N_TilesPerDimension; row++) {
        std::vector<Tile> tileRow;

        for (int column = 0; column < N_TilesPerDimension; column++) {
            tileRow.emplace_back(row, column);
        };
        this->tiles.push_back(tileRow);
    };
};
