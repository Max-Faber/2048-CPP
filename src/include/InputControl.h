#pragma once

#include <cstdio>
#include <GameState.h>
#include <GLFW/glfw3.h>

class InputControl
{
public:
    static bool redrawRequired;

    static void keyboardControl(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void resizeControl(GLFWwindow* window, int width, int height) { redrawRequired = true; }
};
