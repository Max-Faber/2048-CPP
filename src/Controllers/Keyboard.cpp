#include <Controllers/Keyboard.h>

bool Keyboard::redrawRequired = true, Keyboard::tilesMoved = false, Keyboard::pauseRendering = false;

void Keyboard::keyboardControl(GLFWwindow* window, int key, int scancode, int action, int mods)
{
//    if (redrawRequired) return;
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(window, GL_TRUE); return; }
    switch (key)
    {
        case GLFW_KEY_UP:
//            printf("Up\n");
            tilesMoved = GameState::gameState->makeMove(Move::up, true).first;
            break;
        case GLFW_KEY_RIGHT:
//            printf("Right\n");
            tilesMoved = GameState::gameState->makeMove(Move::right, true).first;
            break;
        case GLFW_KEY_DOWN:
//            printf("Down\n");
            tilesMoved = GameState::gameState->makeMove(Move::down, true).first;
            break;
        case GLFW_KEY_LEFT:
//            printf("Left\n");
            tilesMoved = GameState::gameState->makeMove(Move::left, true).first;
            break;
        case GLFW_KEY_SPACE:
//            printf("Spacebar\n");
            pauseRendering = !pauseRendering;
//            printf("pauseRendering: %s\n", pauseRendering ? "true" : "false");
            break;
        default:
            return;
    }
    redrawRequired = tilesMoved;
}
