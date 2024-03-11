#include <Controllers/Keyboard.h>

bool Keyboard::redrawRequired = true, Keyboard::tilesMoved = false, Keyboard::pauseRendering = false, Keyboard::resumeRendering = false;

void Keyboard::keyboardControl(GLFWwindow* window, int key, int scancode, int action, int mods)
{
//    if (redrawRequired) return;
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(window, GL_TRUE); return; }
    switch (key)
    {
        case GLFW_KEY_UP:
            tilesMoved = GameState::gameState->makeMove(Move::up, true).first;
            break;
        case GLFW_KEY_RIGHT:
            tilesMoved = GameState::gameState->makeMove(Move::right, true).first;
            break;
        case GLFW_KEY_DOWN:
            tilesMoved = GameState::gameState->makeMove(Move::down, true).first;
            break;
        case GLFW_KEY_LEFT:
            tilesMoved = GameState::gameState->makeMove(Move::left, true).first;
            break;
        case GLFW_KEY_SPACE:
            if (pauseRendering)
            {
                pauseRendering = false;
                resumeRendering = true;
            }
            else pauseRendering = true;
            break;
        default:
            return;
    }
    redrawRequired = tilesMoved;
}
