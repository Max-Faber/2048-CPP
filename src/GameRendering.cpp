#include <GameRendering.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

colorClamp* GameRendering::bgColor     = new colorClamp(220, 220, 220);
colorClamp* GameRendering::gridBgColor = new colorClamp(178, 168, 158);
colorClamp* GameRendering::gridColor   = new colorClamp(204, 192, 180);

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
    init();
    glutMainLoop();
}

void GameRendering::init()
{
    glutCreateWindow("2048-CPP");
    glutDisplayFunc(display);
    glutSpecialFunc(KeyboardControl);
    calcTilePositions();
    font = new freetype::font_data();
    font->init("fonts/arial.ttf");
}

void GameRendering::calcTilePositions()
{
    float minLength = FLT_MAX, maxLength = -FLT_MAX;

    glColor3f(gridColor->R, gridColor->G, gridColor->B);
    for (int column = 0; column < GameLogic::gridDimension; column++)
    {
        for (int row = 0; row < GameLogic::gridDimension; row++)
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
                    glm::vec2(x - lenTileMarHalf, y - lenTileMarHalf)
            );
        }
    }
    tileContainerLength = abs(minLength) + abs(maxLength);
}

void GameRendering::display()
{
    float curAspectRatio = (float)(curWidth = glutGet(GLUT_WINDOW_WIDTH)) / (float)(curHeight = glutGet(GLUT_WINDOW_HEIGHT));

    glClearColor(bgColor->R, bgColor->G, bgColor->B, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-curAspectRatio, curAspectRatio, minScreenRange, maxScreenRange);
    gluPerspective(0, (float)curWidth / (float)curHeight, 0.01f, 100.f);
    checkAspectRatio();
    drawGame();
    glFlush(); // Flush drawing command buffer to make drawing happen as soon as possible.
}

void GameRendering::checkAspectRatio()
{
    float curAspectRatio = (float)(curWidth) / (float)(curHeight);

    if (curAspectRatio != aspectRatio)
    {
        int newWidth = (int)((float)curHeight * aspectRatio);

        glutReshapeWindow(newWidth, curHeight);
    }
}

void GameRendering::drawGame()
{
    float tileLengthStartEndPixels = tileContainerLength * (float)GameRendering::curHeight;
    unsigned int font_size = (int)(tileLengthStartEndPixels / 62.5f);
    const char* text = "Test 2048!";

    GameRendering::displayGridBackground();
    GameRendering::displayGrid();

    glm::vec2 pos = glm::vec2((float)curWidth * 0.5f, (float)curHeight * 0.05f);
    glm::vec2 newPos = ((pos / glm::vec2(curWidth, curHeight)) * glm::vec2(2, 2)) - glm::vec2(1, 1);
    freetype::renderText(font, font_size, newPos.x, newPos.y, text, curWidth, curHeight);
}

void GameRendering::displayGridBackground()
{
    glColor3f(gridBgColor->R, gridBgColor->G, gridBgColor->B);
    Graphics::drawFilledRoundedRect(*tLeftGridBG, *tRightGridBG, *bRightGridBG, *bLeftGridBG);
}

void GameRendering::displayGrid()
{
    glColor3f(gridColor->R, gridColor->G, gridColor->B);
    for (const std::pair<const int, std::map<const int, rectPosition*>>& row : tilePositions)
    {
        for (const std::pair<const int, rectPosition*>& column : row.second)
        {
            rectPosition* rPos = column.second;

            Graphics::drawFilledRoundedRect(rPos->tLeft, rPos->tRight, rPos->bRight, rPos->bLeft);
        }
    }
}
