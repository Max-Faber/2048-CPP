#pragma once

#ifdef __APPLE_CC__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#define INITIAL_WIDTH 500
#define INITIAL_HEIGHT 700
#define RGB_COLOR_RANGE 255

namespace Graphics {
    const GLfloat backgroundColor       =  220.0f / RGB_COLOR_RANGE;
    const GLfloat gridBackgroundColor[] = {178.0f / RGB_COLOR_RANGE,
                                           168.0f / RGB_COLOR_RANGE,
                                           158.0f / RGB_COLOR_RANGE};

    void init(int argc, char** argv);
    void show();
    void display();
};
