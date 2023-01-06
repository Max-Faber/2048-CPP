#include <GameRendering.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

colorClamp* GameRendering::bgColor          = new colorClamp(220, 220, 220);
colorClamp* GameRendering::gridBgColor      = new colorClamp(178, 168, 158);
colorClamp* GameRendering::gridColor        = new colorClamp(204, 192, 180);
colorClamp* GameRendering::textColorGray    = new colorClamp(110, 100,  92);
colorClamp* GameRendering::textColorWhite   = new colorClamp(255, 255, 255);
std::map<const unsigned int, colorClamp*> GameRendering::tileColors = {
        { pow(BASE_NUMBER,  1), new colorClamp(235, 224, 212) }, //    2
        { pow(BASE_NUMBER,  2), new colorClamp(235, 220, 192) }, //    4
        { pow(BASE_NUMBER,  3), new colorClamp(244, 166, 111) }, //    8
        { pow(BASE_NUMBER,  4), new colorClamp(238, 128,  75) }, //   16
        { pow(BASE_NUMBER,  5), new colorClamp(250, 109,  85) }, //   32
        { pow(BASE_NUMBER,  6), new colorClamp(235,  72,  50) }, //   64
        { pow(BASE_NUMBER,  7), new colorClamp(245, 211,  97) }, //  128
        { pow(BASE_NUMBER,  8), new colorClamp(242, 201,  68) }, //  256
        { pow(BASE_NUMBER,  9), new colorClamp(228, 184,  41) }, //  512
        { pow(BASE_NUMBER, 10), new colorClamp(225, 177,  25) }, // 1024
        { pow(BASE_NUMBER, 11), new colorClamp(236, 188,  16) }, // 2048
        { pow(BASE_NUMBER, 12), new colorClamp( 70, 215, 135) }  // 4096
};

unsigned int GameRendering::fontSize;

float GameRendering::tileContainerLength;

glm::vec2* GameRendering::tLeftGridBG  = new glm::vec2(-gridRangeHalf,  gridRangeHalf - vertGridRangeOffsHalf);
glm::vec2* GameRendering::tRightGridBG = new glm::vec2( gridRangeHalf,  gridRangeHalf - vertGridRangeOffsHalf);
glm::vec2* GameRendering::bRightGridBG = new glm::vec2( gridRangeHalf, -gridRangeHalf - vertGridRangeOffsHalf);
glm::vec2* GameRendering::bLeftGridBG  = new glm::vec2(-gridRangeHalf, -gridRangeHalf - vertGridRangeOffsHalf);

std::map<const int, std::map<const int, rectPosition*>> GameRendering::tilePositions;

freetype::font_data* GameRendering::font;

int GameRendering::curWidth, GameRendering::curHeight;

void GameRendering::show()
{
    int a = 0;

    init();
    glfwSetWindowAspectRatio(Graphics::window, initialWidth, initialHeight);
    while (!glfwWindowShouldClose(Graphics::window))
    {
        glfwWaitEvents();
        if (!InputControl::redrawRequired) continue;
        display();
        glfwSwapBuffers(Graphics::window);
        printf("a: %d\n", a+=1);
        InputControl::redrawRequired = false;
    }
    glfwDestroyWindow(Graphics::window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void GameRendering::init()
{
    calcTilePositions();
    font = new freetype::font_data();
    // font->init("fonts/arial.ttf");
    font->init("fonts/SmallMemory.ttf");
}

void GameRendering::calcTilePositions()
{
    float minLength = FLT_MAX, maxLength = -FLT_MAX;

    for (int column = 0; column < GameState::gridDimension; column++)
    {
        for (int row = 0; row < GameState::gridDimension; row++)
        {
            const float offs    =  lenTileMarHalf + lenTotMar;
            const float horOffs =  offs;
            const float verOffs =  offs + vertGridRangeOffsHalf;
            const float x       = -gridRangeHalf + (float)row    * (lenTileMar + lenTotMar) + horOffs;
            const float y       =  gridRangeHalf - (float)column * (lenTileMar + lenTotMar) - verOffs;

            minLength = std::min(minLength, x - lenTileMarHalf);
            maxLength = std::max(maxLength, x + lenTileMarHalf);
            tilePositions[row][column] = new rectPosition(
                    glm::vec2(x - lenTileMarHalf, y + lenTileMarHalf),
                    glm::vec2(x + lenTileMarHalf, y + lenTileMarHalf),
                    glm::vec2(x + lenTileMarHalf, y - lenTileMarHalf),
                    glm::vec2(x - lenTileMarHalf, y - lenTileMarHalf),
                    glm::vec2(x, y)
            );
        }
    }
    tileContainerLength = abs(minLength) + abs(maxLength);
}

void GameRendering::display()
{
    glfwGetWindowSize(Graphics::window, &curWidth, &curHeight);
    float curAspectRatio = (float)(curWidth) / (float)(curHeight);

    glClearColor(bgColor->R, bgColor->G, bgColor->B, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-curAspectRatio, curAspectRatio, minScreenRange, maxScreenRange, -1, 1);
    drawGame();
    glFlush(); // Flush drawing command buffer to make drawing happen as soon as possible.
}

void GameRendering::drawGame()
{
    float tileLengthStartEndPixels = tileContainerLength * (float)GameRendering::curHeight;
//    double lastTime = glfwGetTime();

    fontSize = (int)(tileLengthStartEndPixels / 15.f);
    displayGridBackground();
    displayGrid();
}

void GameRendering::displayGridBackground()
{
    glColor3f(gridBgColor->R, gridBgColor->G, gridBgColor->B);
    Graphics::drawFilledRoundedRect(*tLeftGridBG, *tRightGridBG, *bRightGridBG, *bLeftGridBG);
}

void GameRendering::displayGrid()
{
    for (const std::pair<const int, std::map<const int, rectPosition*>>& row : tilePositions)
    {
        for (const std::pair<const int, rectPosition*>& column : row.second) {
//            FieldPos *fPos = GameState::fieldTileRows[row.first][column.first];
            rectPosition *rPos = column.second;

            glColor3f(gridColor->R, gridColor->G, gridColor->B);
            Graphics::drawFilledRoundedRect(rPos->tLeft, rPos->tRight, rPos->bRight, rPos->bLeft);
        }
    }
    for (const std::pair<const int, std::map<const int, rectPosition*>>& row : tilePositions)
    {
        for (const std::pair<const int, rectPosition*>& column : row.second)
        {
            FieldPos* fPos = GameState::fieldTileRows[row.first][column.first];
            rectPosition* rPos = column.second;

//            glColor3f(gridColor->R, gridColor->G, gridColor->B);
//            Graphics::drawFilledRoundedRect(rPos->tLeft, rPos->tRight, rPos->bRight, rPos->bLeft);
            if (!fPos->tile) continue;
            drawTile(fPos, row.first, column.first);
        }
    }
}

void GameRendering::drawTile(FieldPos* fPos, int x, int y)
{
    std::map<const unsigned int, colorClamp*>::iterator colorIt = tileColors.find(fPos->tile->val);
    rectPosition* rPos = tilePositions[fPos->y][fPos->x];
    std::string valStr = std::to_string(fPos->tile->val);
    unsigned long valStrSize = valStr.size();
    float fSize = (float)fontSize;
    colorClamp* color;
    TransitionInfo* tInfo = GameState::transitions[y][x];
    rectPosition posTrans = rectPosition(rPos->tLeft, rPos->tRight, rPos->bRight, rPos->bLeft, rPos->center);

    if (colorIt == tileColors.end()) colorIt = tileColors.find(2048);
    color = (*colorIt).second;
    glColor3f(color->R, color->G, color->B);
    if (tInfo)
    {
        rectPosition posSource = *tilePositions[tInfo->source->y][tInfo->source->x];
        rectPosition posTarget = *tilePositions[tInfo->target->y][tInfo->target->x];
        rectPosition totDistance = posTarget - posSource;
        posTrans = rectPosition(posSource.tLeft, posSource.tRight, posSource.bRight, posSource.bLeft, posSource.center);
        printf("Source (%d, %d)\n", x, y);

        posTrans.tLeft += glm::vec2(0.5, 0.5) * totDistance.tLeft;
        posTrans.tRight += glm::vec2(0.5, 0.5) * totDistance.tRight;
        posTrans.bRight += glm::vec2(0.5, 0.5) * totDistance.bRight;
        posTrans.bLeft += glm::vec2(0.5, 0.5) * totDistance.bLeft;
        posTrans.center += glm::vec2(0.5, 0.5) * totDistance.center;
        int a = 5;
        GameState::transitions[y].erase(x);
        GameState::transitions.erase(y);
    }
    Graphics::drawFilledRoundedRect(posTrans.tLeft, posTrans.tRight, posTrans.bRight, posTrans.bLeft);
    fSize *= valStrSize >= 3 ? 2.75f / (float)valStrSize : 1;
    color = fPos->tile->val <= 4 ? textColorGray : textColorWhite;
    glColor3f(color->R, color->G, color->B);
    freetype::renderText(
            font,
            (int)round(fSize),
            rPos->center.x,
            rPos->center.y,
            valStr.c_str(),
            curWidth,
            curHeight
    );
}
