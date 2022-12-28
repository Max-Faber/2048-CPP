#include <GameRendering.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

colorClamp* GameRendering::bgColor     = new colorClamp(220, 220, 220);
colorClamp* GameRendering::gridBgColor = new colorClamp(178, 168, 158);
colorClamp* GameRendering::gridColor   = new colorClamp(204, 192, 180);
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
                    glm::vec2(x - lenTileMarHalf, y - lenTileMarHalf),
                    glm::vec2(x, y)
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
    fontSize = (int)(tileLengthStartEndPixels / 15.f);

    displayGridBackground();
    displayGrid();
    displayColoredGrid();

    glm::vec2 pos = glm::vec2((float)curWidth * 0.2f, (float)curHeight * 0.2f);
    glm::vec2 newPos = ((pos / glm::vec2(curWidth, curHeight)) * glm::vec2(2, 2)) - glm::vec2(1, 1);
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
            FieldPos* fPos = GameLogic::fieldTileRows[row.first][column.first];
            rectPosition* rPos = column.second;


            glColor3f(gridColor->R, gridColor->G, gridColor->B);
            Graphics::drawFilledRoundedRect(rPos->tLeft, rPos->tRight, rPos->bRight, rPos->bLeft);
        }
    }
}

void GameRendering::displayColoredGrid()
{
    for (const std::pair<const int, std::map<const int, FieldPos*>>& row : GameLogic::fieldTileRows)
    {
        for (std::pair<const int, FieldPos*> column : row.second)
        {
            FieldPos* pos = column.second;
            rectPosition* rPos = tilePositions[pos->y][pos->x];
            float fSize = (float)fontSize;

            if (!pos->tile) continue;
            std::string valStr = std::to_string(pos->tile->val);
            colorClamp* color = tileColors[pos->tile->val];

            glColor3f(color->R, color->G, color->B);
            Graphics::drawFilledRoundedRect(rPos->tLeft, rPos->tRight, rPos->bRight, rPos->bLeft);
            if (valStr.size() > 2) fSize = fSize * (float)(2.75 / ((float)(float)valStr.size()));
            freetype::renderText(
                    font,
                    fSize,
                    rPos->center.x,
                    rPos->center.y,
                    valStr.c_str(),
                    curWidth,
                    curHeight
            );
        }
    }
}
