#pragma once

#include <Tile.h>
#include <vector>
#ifdef __APPLE_CC__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#define N_TilesPerDimension 4
#define N_Tiles N_TilesPerDimension * N_TilesPerDimension

class GameMechanics
{
public:
    GameMechanics();
private:
    std::vector<std::vector<Tile>> tiles;

    void initializeTiles();
};
