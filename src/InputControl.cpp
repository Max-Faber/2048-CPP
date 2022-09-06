#include <InputControl.h>

void KeyboardControl(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            printf("Up\n");
            GameLogic::mergeTiles(GameLogic::fieldTileColumns);
            break;
        case GLUT_KEY_DOWN:
            printf("Down\n");
            GameLogic::mergeTiles(GameLogic::fieldTileColumns);
            break;
        case GLUT_KEY_LEFT:
            printf("Left\n");
            GameLogic::mergeTiles(GameLogic::fieldTileColumns);
            break;
        case GLUT_KEY_RIGHT:
            printf("Right\n");
            GameLogic::mergeTiles(GameLogic::fieldTileColumns);
            break;
        default:
            return;
    }
}
