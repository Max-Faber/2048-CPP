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

float GameRendering::tileContainerLength, GameRendering::transitionFrac;

glm::vec2* GameRendering::tLeftGridBG  = new glm::vec2(-gridRangeHalf,  gridRangeHalf - vertGridRangeOffsHalf);
glm::vec2* GameRendering::tRightGridBG = new glm::vec2( gridRangeHalf,  gridRangeHalf - vertGridRangeOffsHalf);
glm::vec2* GameRendering::bRightGridBG = new glm::vec2( gridRangeHalf, -gridRangeHalf - vertGridRangeOffsHalf);
glm::vec2* GameRendering::bLeftGridBG  = new glm::vec2(-gridRangeHalf, -gridRangeHalf - vertGridRangeOffsHalf);

std::map<const int, std::map<const int, rectPosition*>> GameRendering::tilePositions;

freetype::font_data* GameRendering::font;

int GameRendering::curWidth, GameRendering::curHeight;

void GameRendering::show()
{
    int drawCount = 0;

    init();
    glfwSetWindowAspectRatio(Graphics::window, initialWidth, initialHeight);
    while (!glfwWindowShouldClose(Graphics::window))
    {
        glfwWaitEvents();
        if (!InputControl::redrawRequired) continue;
        display();
        printf("drawCount: %d\n", drawCount+=1);
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
    float curAspectRatio;

    glfwGetWindowSize(Graphics::window, &curWidth, &curHeight);
    curAspectRatio = (float)(curWidth) / (float)(curHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-curAspectRatio, curAspectRatio, minScreenRange, maxScreenRange, -1, 1);
    drawGame();
//    drawNewTile();
}

void GameRendering::drawGame()
{
    float tileLengthStartEndPixels = tileContainerLength * (float)GameRendering::curHeight;
    float totSecs = .1;
    const int limitFPS = 120, totFrames = (int)(totSecs * limitFPS);
    double timeStepSeconds = totSecs / (double)totFrames;
    int fpsCnt = 0;

    fontSize = (int)(tileLengthStartEndPixels / 15.f);
    glfwSetTime(0.);
    double curTime;

    double deadline = 0.;
    while (true/*(curTime = glfwGetTime()) < totSecs*/)
    {
        if (glfwWindowShouldClose(Graphics::window)) break;
        if ((transitionFrac = (float)glfwGetTime() / totSecs) > 1) transitionFrac = 1;
        glClearColor(bgColor->R, bgColor->G, bgColor->B, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        displayGridBackground();
        displayGrid();
        fpsCnt += 1;
        curTime = glfwGetTime();
        deadline += timeStepSeconds;
        if (limitFPS != -1 && curTime < deadline)
        {
            usleep((uint)((deadline - curTime) * 1000000));
            /*printf("curTime: %f\n", curTime); */
        }
        if (transitionFrac == 1.) break;

//        printf("deadline: %f\n", deadline);



        glfwSwapBuffers(Graphics::window);
        glfwPollEvents();
    }
    double test = (double)fpsCnt / glfwGetTime();

    int a = (int)test;
    int b = ceil(test);
    printf("Avg fps: %d\n", (int)ceil( ((double)fpsCnt / glfwGetTime()) ) );
    drawNewTile(); fpsCnt++;
    // Cleanup transitions and merges
    for (const std::pair<const int, std::map<const int, TransitionInfo *>>& tInfoMap : GameState::transitions)
    {
        for (std::pair<const int, TransitionInfo*> tInfo : tInfoMap.second)
        {
            // if (!tInfo.second) continue;
            delete tInfo.second;
            tInfo.second = nullptr;
        }
        GameState::transitions[tInfoMap.first].clear();
    }
    GameState::transitions.clear();
    for (const std::pair<const int, std::map<const int, TransitionInfo *>>& tInfoMap : GameState::merges)
    {
        for (std::pair<const int, TransitionInfo*> tInfo : tInfoMap.second)
        {
//            if (!tInfo.second) continue;
            delete tInfo.second;
            tInfo.second = nullptr;
        }
        GameState::merges[tInfoMap.first].clear();
    }
    GameState::merges.clear();
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
            FieldPos* fPos = GameState::fieldTileRows[row.first][column.first];

            if (!fPos->tile) continue;
            drawTile(fPos, row.first, column.first);
        }
    }
}

void GameRendering::drawNewTile()
{
    FieldPos* fPos = GameState::spawnTileRandom();

    GameState::printGrid();
    drawTile(fPos, fPos->x, fPos->y);
    glfwSwapBuffers(Graphics::window);
}

void GameRendering::drawTile(FieldPos* fPos, int x, int y)
{
    std::map<const unsigned int, colorClamp*>::iterator colorIt = tileColors.find(fPos->tile->val);
    rectPosition* rPos = tilePositions[fPos->y][fPos->x];
    std::string valStr = std::to_string(fPos->tile->val);
    unsigned long valStrSize = valStr.size();
    float fSize = (float)fontSize;
    colorClamp* color;
    TransitionInfo* tInfo = TransitionInfo::getTransitionInfo(fPos->x, fPos->y);
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

        posTrans.tLeft  += glm::vec2(transitionFrac, transitionFrac) * totDistance.tLeft;
        posTrans.tRight += glm::vec2(transitionFrac, transitionFrac) * totDistance.tRight;
        posTrans.bRight += glm::vec2(transitionFrac, transitionFrac) * totDistance.bRight;
        posTrans.bLeft  += glm::vec2(transitionFrac, transitionFrac) * totDistance.bLeft;
        posTrans.center += glm::vec2(transitionFrac, transitionFrac) * totDistance.center;
    }
    Graphics::drawFilledRoundedRect(posTrans.tLeft, posTrans.tRight, posTrans.bRight, posTrans.bLeft);
    fSize *= valStrSize >= 3 ? 2.75f / (float)valStrSize : 1;
    color = fPos->tile->val <= 4 ? textColorGray : textColorWhite;
    glColor3f(color->R, color->G, color->B);
    freetype::renderText( // Memory leak
            font,
            (int)round(fSize),
            posTrans.center.x,
            posTrans.center.y,
            valStr.c_str(),
            curWidth,
            curHeight
    );
}
