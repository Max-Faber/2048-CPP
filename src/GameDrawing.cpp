#include <GameDrawing.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

colorClamp* GameDrawing::bgColor     = new colorClamp(220, 220, 220);
colorClamp* GameDrawing::gridBgColor = new colorClamp(178, 168, 158);
colorClamp* GameDrawing::gridColor   = new colorClamp(204, 192, 180);

vec2* GameDrawing::tLeftGridBG  = new vec2(-gridRangeHalf, gridRangeHalf - vertGridRangeOffsHalf);
vec2* GameDrawing::tRightGridBG = new vec2(gridRangeHalf, gridRangeHalf - vertGridRangeOffsHalf);
vec2* GameDrawing::bRightGridBG = new vec2(gridRangeHalf, -gridRangeHalf - vertGridRangeOffsHalf);
vec2* GameDrawing::bLeftGridBG  = new vec2(-gridRangeHalf, -gridRangeHalf - vertGridRangeOffsHalf);

void GameDrawing::show() {
    glutCreateWindow("2048-CPP");
    glutDisplayFunc(display);
    glutMainLoop();
}

void GameDrawing::display() {
    int width         = glutGet(GLUT_WINDOW_WIDTH), height = glutGet(GLUT_WINDOW_HEIGHT);
    float aspectRatio = (float)width / (float)height;

    glClearColor(bgColor->R, bgColor->G, bgColor->B, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-aspectRatio, aspectRatio, minScreenRange, maxScreenRange);
    drawGame();
    glFlush(); // Flush drawing command buffer to make drawing happen as soon as possible.
}

void GameDrawing::drawGame() {
    GameDrawing::displayGridBackground();
    GameDrawing::displayGrid();
}

void GameDrawing::displayGridBackground() {
    glColor3f(gridBgColor->R, gridBgColor->G, gridBgColor->B);
    Graphics::drawFilledRoundedRect(*tLeftGridBG, *tRightGridBG, *bRightGridBG, *bLeftGridBG);
}

void GameDrawing::displayGrid() {
    glColor3f(gridColor->R, gridColor->G, gridColor->B);
    for (int column = 0; column < GameLogic::gridDimension; column++) {
        for (int row = 0; row < GameLogic::gridDimension; row++) {
            const float offs    =  lenTileMarHalf + lenTotMar;
            const float horOffs =  offs;
            const float verOffs =  offs + vertGridRangeOffsHalf;
            const float x       = -gridRangeHalf + (float)row    * (lenTileMar + lenTotMar) + horOffs;
            const float y       =  gridRangeHalf - (float)column * (lenTileMar + lenTotMar) - verOffs;

            Graphics::drawFilledRoundedRect(
                    vec2(x - lenTileMarHalf, y + lenTileMarHalf),
                    vec2(x + lenTileMarHalf, y + lenTileMarHalf),
                    vec2(x + lenTileMarHalf, y - lenTileMarHalf),
                    vec2(x - lenTileMarHalf, y - lenTileMarHalf)
            );
        }
    }
}
