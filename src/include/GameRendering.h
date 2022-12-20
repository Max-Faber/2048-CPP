#pragma once

#include <cfloat>

#include <TextRendering.h>
#include <Graphics.h>
#include <GameLogic.h>
#include <InputControl.h>

#define ABS(N) ((N<0) ? (-N) : (N))

class GameRendering
{
private:
    static const int minScreenRange = -1;
    static const int maxScreenRange =  1;
    static const int vertScaleSize  = ABS(minScreenRange) + ABS(minScreenRange);
    static unsigned int fontSize;

    constexpr static const float gridRange             = 1.1f;
    constexpr static const float gridRangeHalf         = gridRange * .5f;
    constexpr static const float lenTileFull           = gridRange / GameLogic::gridDimension;
    constexpr static const float lenTotMar             = lenTileFull * .1f;
    constexpr static const float lenTileMar            = lenTileFull - (lenTotMar * ((1.0f + GameLogic::gridDimension) / GameLogic::gridDimension));
    constexpr static const float lenTileMarHalf        = lenTileMar * .5f;
    constexpr static const float vertGridRangeOffsHalf = (gridRange / (float)vertScaleSize) * .5f;

    static float tileContainerLength;

    static colorClamp* bgColor;
    static colorClamp* gridBgColor;
    static colorClamp* gridColor;

    static glm::vec2* tLeftGridBG;
    static glm::vec2* tRightGridBG;
    static glm::vec2* bRightGridBG;
    static glm::vec2* bLeftGridBG;

    static std::map<const int, std::map<const int, rectPosition*>> tilePositions;

    static freetype::font_data* font;

    static void init();
    static void calcTilePositions();
    static void checkAspectRatio();
    static void drawGame();
    static void displayGridBackground();
    static void displayGrid();
public:
    constexpr static const int initialWidth  =  500;
    constexpr static const int initialHeight =  700;
    constexpr static const float aspectRatio = (float)initialWidth / (float)initialHeight;
    static int curWidth, curHeight;

    static void show();
    static void display();
};
