#include <Controllers/Keyboard.h>

bool Keyboard::redrawRequired = true, Keyboard::tilesMoved = false;

void Keyboard::keyboardControl(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (redrawRequired) return;
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(window, GL_TRUE); return; }
    switch (key)
    {
        case GLFW_KEY_UP:
            printf("Up\n");
            tilesMoved = GameState::makeMove(Move::up).first;
            break;
        case GLFW_KEY_RIGHT:
            printf("Right\n");
            tilesMoved = GameState::makeMove(Move::right).first;
            break;
        case GLFW_KEY_DOWN:
            printf("Down\n");
            tilesMoved = GameState::makeMove(Move::down).first;
            break;
        case GLFW_KEY_LEFT:
            printf("Left\n");
            tilesMoved = GameState::makeMove(Move::left).first;
            break;
        default:
            return;
    }
    redrawRequired = tilesMoved;
}
