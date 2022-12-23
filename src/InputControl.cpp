#include <InputControl.h>

void KeyboardControl(int key, int x, int y)
{
    bool tilesMoved = false;

    switch (key) {
        case GLUT_KEY_UP:
            printf("Up\n");
            tilesMoved = GameLogic::mergeTiles(GameLogic::fieldTileRows);
            break;
        case GLUT_KEY_DOWN:
            printf("Down\n");
            tilesMoved = GameLogic::mergeTiles(GameLogic::fieldTileRowsReversed);
            break;
        case GLUT_KEY_LEFT:
            printf("Left\n");
            tilesMoved = GameLogic::mergeTiles(GameLogic::fieldTileColumns);
            break;
        case GLUT_KEY_RIGHT:
            printf("Right\n");
            tilesMoved = GameLogic::mergeTiles(GameLogic::fieldTileColumnsReversed);
            break;
        default:
            return;
    }
    if (tilesMoved) GameLogic::spawnTileRandom();
    GameLogic::printGrid();
    GameRendering::display();
}
