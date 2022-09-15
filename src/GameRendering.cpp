#include <GameRendering.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

colorClamp* GameRendering::bgColor     = new colorClamp(220, 220, 220);
colorClamp* GameRendering::gridBgColor = new colorClamp(178, 168, 158);
colorClamp* GameRendering::gridColor   = new colorClamp(204, 192, 180);

vec2* GameRendering::tLeftGridBG  = new vec2(-gridRangeHalf, gridRangeHalf - vertGridRangeOffsHalf);
vec2* GameRendering::tRightGridBG = new vec2(gridRangeHalf, gridRangeHalf - vertGridRangeOffsHalf);
vec2* GameRendering::bRightGridBG = new vec2(gridRangeHalf, -gridRangeHalf - vertGridRangeOffsHalf);
vec2* GameRendering::bLeftGridBG  = new vec2(-gridRangeHalf, -gridRangeHalf - vertGridRangeOffsHalf);

void GameRendering::show() {
    glutCreateWindow("2048-CPP");
    glutDisplayFunc(display);
    glutSpecialFunc(KeyboardControl);
    glutMainLoop();
}

void GameRendering::display() {
    float aspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);

    glClearColor(bgColor->R, bgColor->G, bgColor->B, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-aspectRatio, aspectRatio, minScreenRange, maxScreenRange);
    drawGame();
    glFlush(); // Flush drawing command buffer to make drawing happen as soon as possible.
}

void GameRendering::drawGame() {
    GameRendering::displayGridBackground();
    GameRendering::displayGrid();


    freetype::font_data our_font;
    our_font.init("fonts/arial.ttf", 16);
    freetype::print(our_font, 320, 200, "Active FreeType Text - %7.2f", 0);
}

void GameRendering::displayGridBackground() {
    glColor3f(gridBgColor->R, gridBgColor->G, gridBgColor->B);
    Graphics::drawFilledRoundedRect(*tLeftGridBG, *tRightGridBG, *bRightGridBG, *bLeftGridBG);
}

void GameRendering::displayGrid() {
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
