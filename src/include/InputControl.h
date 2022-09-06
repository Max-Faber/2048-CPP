#pragma once

#include <cstdio>
#include <GameLogic.h>
#ifdef __APPLE_CC__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

void KeyboardControl(int key, int x, int y);
