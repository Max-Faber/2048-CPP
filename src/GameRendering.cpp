#include <GameRendering.h>

#ifndef WIN32
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#endif

colorClamp *GameRendering::bgColor          = new colorClamp(220, 220, 220);
colorClamp *GameRendering::gridBgColor      = new colorClamp(178, 168, 158);
colorClamp *GameRendering::gridColor        = new colorClamp(204, 192, 180);
colorClamp *GameRendering::textColorGray    = new colorClamp(110, 100,  92);
colorClamp *GameRendering::textColorWhite   = new colorClamp(255, 255, 255);
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

float GameRendering::tileContainerLength, GameRendering::transitionFrac;

glm::vec2* GameRendering::tLeftGridBG  = new glm::vec2(-gridRangeHalf, gridRangeHalf - gridMargin);
glm::vec2* GameRendering::tRightGridBG = new glm::vec2( gridRangeHalf, gridRangeHalf - gridMargin);
glm::vec2* GameRendering::bRightGridBG = new glm::vec2( gridRangeHalf, -gridRangeHalf - gridMargin);
glm::vec2* GameRendering::bLeftGridBG  = new glm::vec2(-gridRangeHalf, -gridRangeHalf - gridMargin);

std::map<const int, std::map<const int, rectPosition*>> GameRendering::tilePositions;

fontData* GameRendering::font;

int GameRendering::curWidth, GameRendering::curHeight;

void GameRendering::show()
{
    int drawCount = 0;

    printf("Show\n");

    init();
    glfwSetWindowAspectRatio(Graphics::window, initialWidth, initialHeight);
    return;
    while (!glfwWindowShouldClose(Graphics::window))
    {
        glfwWaitEvents();
        if (!Keyboard::redrawRequired) continue;
        Keyboard::redrawRequired = false;
        display();
        printf("drawCount: %d\n", drawCount+=1);
    }
    glfwDestroyWindow(Graphics::window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void GameRendering::init()
{
    calcTilePositions();
    font = new fontData();
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
            const float verOffs = offs + gridMargin;
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
    float curAspectRatio;

    if (Keyboard::pauseRendering)
    {
        glfwPollEvents();
        if (Keyboard::tilesMoved) GameState::gameState->spawnTileRandom();
        GameState::gameState->cleanTransitionInfo();
        return;
    }
    glfwGetWindowSize(Graphics::window, &curWidth, &curHeight);
    curAspectRatio = (float)(curWidth) / (float)(curHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-curAspectRatio, curAspectRatio, minScreenRange, maxScreenRange, -1, 1);
    drawGame();
    GameState::gameState->cleanTransitionInfo();
}

void GameRendering::drawGame()
{
    float tileLengthStartEndPixels = tileContainerLength * (float)GameRendering::curHeight, totSecs = .05;
    const int limitFPS = 60000, totFrames = (int)(totSecs * limitFPS);
    double timeStepSeconds = totSecs / (double)totFrames, deadline = 0., curTime;
    int fpsCnt = 0;

    font->fontSize = (int)(tileLengthStartEndPixels / 15.f);
    glfwSetTime(0.);
    while (true)
    {
        if (glfwWindowShouldClose(Graphics::window)) break;
        if ((transitionFrac = (float)glfwGetTime() / totSecs) > 1) transitionFrac = 1;
        glClearColor(bgColor->R, bgColor->G, bgColor->B, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        displayGridBackground();
        displayGrid();
        drawScoreBoard();
        fpsCnt += 1;
        curTime = glfwGetTime();
        deadline += timeStepSeconds;
        if (limitFPS != -1 && curTime < deadline)
        {
            unsigned int sleepTime = (unsigned int)((deadline - curTime) * 1000000);

#ifdef _WIN32
            Sleep(sleepTime / 1000);
#else
            usleep(sleepTime);
#endif
        }
        if (transitionFrac == 1.) break;
        glfwSwapBuffers(Graphics::window);
        glfwPollEvents();
    }
    // printf("Avg fps: %d\n", (int)ceil( ((double)fpsCnt / glfwGetTime()) ) );
    drawNewTile();
    // Cleanup transitions and merges
    GameState::gameState->cleanTransitionInfo();
//    for (const std::pair<const int, std::map<const int, TransitionInfo *>>& tInfoMap : GameState::transitions)
//    {
//        for (std::pair<const int, TransitionInfo*> tInfo : tInfoMap.second)
//        {
//            if (!tInfo.second) continue;
//            delete tInfo.second;
//            tInfo.second = nullptr;
//        }
//        GameState::transitions[tInfoMap.first].clear();
//    }
//    GameState::transitions.clear();
//    for (const std::pair<const int, std::map<const int, TransitionInfo *>>& tInfoMap : GameState::merges)
//    {
//        for (std::pair<const int, TransitionInfo*> tInfo : tInfoMap.second)
//        {
//            if (!tInfo.second) continue;
//            delete tInfo.second;
//            tInfo.second = nullptr;
//        }
//        GameState::merges[tInfoMap.first].clear();
//    }
//    GameState::merges.clear();
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
        for (const std::pair<const int, rectPosition*>& column : row.second)
        {
            rectPosition *rPos = column.second;

            glColor3f(gridColor->R, gridColor->G, gridColor->B);
            Graphics::drawFilledRoundedRect(rPos->tLeft, rPos->tRight, rPos->bRight, rPos->bLeft);
        }
    }
    for (const std::pair<const int, std::map<const int, rectPosition*>>& row : tilePositions)
    {
        for (const std::pair<const int, rectPosition*>& column : row.second)
        {
            FieldPos* fPos = GameState::gameState->fieldTileRows[row.first][column.first];

            if (!fPos->tile) continue;
            drawTile(fPos);
        }
    }
}

void GameRendering::drawScoreBoard()
{
    glm::vec2 center = glm::vec2(gridRangeHalf / 2.f, (2.f - gridRange - gridMargin) / 2.f);
    float horOffset = gridRangeHalf - center.x;
    float vertOffset = gridRangeHalf - center.y + gridMargin;

    glColor3f(gridBgColor->R, gridBgColor->G, gridBgColor->B);
    Graphics::drawFilledRoundedRect(
            glm::vec2(-center.x - horOffset,  center.y + vertOffset),
            glm::vec2( center.x - horOffset,  center.y + vertOffset),
            glm::vec2( center.x - horOffset, -center.y + vertOffset),
            glm::vec2(-center.x - horOffset, -center.y + vertOffset)
    );
    glColor3f(textColorGray->R, textColorGray->G, textColorGray->B);
    TextRendering::renderText(
            font,
            center.x - horOffset - (center.x / 1.f),
            center.y + vertOffset - (center.y / 2.5f),
            "Score",
            curWidth,
            curHeight
    );
    glColor3f(textColorWhite->R, textColorWhite->G, textColorWhite->B);
    TextRendering::renderText(
            font,
            center.x - horOffset - center.x,
            center.y + vertOffset - center.y,
            std::to_string(GameState::gameState->score),
            curWidth,
            curHeight
    );
}

void GameRendering::drawNewTile()
{
    FieldPos* fPos;

    if (!Keyboard::tilesMoved) return;
    fPos = GameState::gameState->spawnTileRandom();
    // GameState::printGrid();
    drawTile(fPos);
    glfwSwapBuffers(Graphics::window);
    Keyboard::tilesMoved = false;
}

void GameRendering::drawTile(FieldPos *fPos)
{
    std::map<const unsigned int, colorClamp*>::iterator colorIt = tileColors.find(fPos->tile->val);
    rectPosition  *rPos = tilePositions[fPos->y][fPos->x];
    colorClamp *color;
    TransitionInfo *tInfo = GameState::gameState->getTransitionInfo(fPos->x, fPos->y);
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

        posTrans += totDistance * glm::vec2(transitionFrac, transitionFrac);
        // delete tInfo; tInfo = nullptr; GameState::gameState->transitions[fPos->x][fPos->y] = nullptr;
    }
    Graphics::drawFilledRoundedRect(posTrans.tLeft, posTrans.tRight, posTrans.bRight, posTrans.bLeft);
    color = fPos->tile->val <= 4 ? textColorGray : textColorWhite;
    glColor3f(color->R, color->G, color->B);
    TextRendering::renderText(
            font,
            posTrans.center.x,
            posTrans.center.y,
            std::to_string(fPos->tile->val),
            curWidth,
            curHeight
    );
}
