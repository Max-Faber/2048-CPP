#include <Controllers/Keyboard.h>

bool Keyboard::redrawRequired = true, Keyboard::tilesMoved = false;

void Keyboard::keyboardControl(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (redrawRequired) return;
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(window, GL_TRUE); return; }
    switch (key) {
        case GLFW_KEY_UP:
            printf("Up\n");
            tilesMoved = GameState::mergeTiles(GameState::fieldTileRows);
            break;
        case GLFW_KEY_DOWN:
            printf("Down\n");
            tilesMoved = GameState::mergeTiles(GameState::fieldTileRowsReversed);
            break;
        case GLFW_KEY_LEFT:
            printf("Left\n");
            tilesMoved = GameState::mergeTiles(GameState::fieldTileColumns);
            break;
        case GLFW_KEY_RIGHT:
            printf("Right\n");
            tilesMoved = GameState::mergeTiles(GameState::fieldTileColumnsReversed);
            break;
        default:
            return;
    }
    redrawRequired = tilesMoved;
}