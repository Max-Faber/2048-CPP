#include <InputControl.h>

bool InputControl::redrawRequired = true;

void InputControl::keyboardControl(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bool tilesMoved;

    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(window, GL_TRUE); return; }
    switch (key)
    {
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
    GameState::printGrid();
    if (tilesMoved) GameState::spawnTileRandom();
    GameState::printGrid();
    redrawRequired = true;
}

void InputControl::resizeControl(GLFWwindow* window, int width, int height)
{
    redrawRequired = true;
}
