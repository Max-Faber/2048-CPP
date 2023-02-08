#pragma once

#include <cfloat>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <TextRendering.h>
#include <Graphics.h>
#include <GameState.h>

#define BASE_NUMBER 2

class GameRendering
{
public:
    constexpr static const int initialWidth  = 700;
    constexpr static const int initialHeight = 900;
    constexpr static const float aspectRatio = initialWidth < initialHeight ? (float)initialWidth / (float)initialHeight : (float)initialHeight / (float)initialWidth;
    static int curWidth, curHeight;

    static void show();
    static void display();
private:
    static const int minScreenRange = -1;
    static const int maxScreenRange =  1;

    constexpr static const float rangeWidth     = aspectRatio * 2.f;
    constexpr static const float gridRange      = rangeWidth * .9f;
    constexpr static const float gridRangeHalf  = gridRange * .5f;
    constexpr static const float lenTileFull    = gridRange / GameState::gridDimension;
    constexpr static const float lenTotMar      = lenTileFull * .1f;
    constexpr static const float lenTileMar     = lenTileFull - (lenTotMar * ((1.0f + GameState::gridDimension) / GameState::gridDimension));
    constexpr static const float lenTileMarHalf = lenTileMar * .5f;
    constexpr static const float gridMargin     = ((float)initialHeight - (float)initialWidth) / (float)initialHeight;

    static float tileContainerLength, transitionFrac;

    static colorClamp* bgColor;
    static colorClamp* gridBgColor;
    static colorClamp* gridColor;
    static colorClamp* textColorGray;
    static colorClamp* textColorWhite;
    static std::map<const unsigned int, colorClamp*> tileColors;

    static glm::vec2* tLeftGridBG;
    static glm::vec2* tRightGridBG;
    static glm::vec2* bRightGridBG;
    static glm::vec2* bLeftGridBG;

    static std::map<const int, std::map<const int, rectPosition*>> tilePositions;

    static fontData* font;

    static void init();
    static void calcTilePositions();
    static void drawGame();
    static void displayGridBackground();
    static void displayGrid();
    static void drawScoreBoard();
    static void drawNewTile();
    static void drawTile(FieldPos* fPos);
};
