#pragma once

#include <cstdio>
#include <cmath>

#ifdef __APPLE_CC__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#ifdef _WIN32
    #include <corecrt_math_defines.h>
#endif

struct vec2 {
    float x, y;

    vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }
};

struct colorClamp {
    float R, G, B;
    static const int colorRangeRGB = 255;

    colorClamp(float rClamp, float gClamp, float bClamp) {
        this->R = rClamp;
        this->G = gClamp;
        this->B = bClamp;
    }

    colorClamp(int rRGB, int gRGB, int bRGB) {
        this->R = (float)rRGB / colorRangeRGB;
        this->G = (float)gRGB / colorRangeRGB;
        this->B = (float)bRGB / colorRangeRGB;
    }
};

class Graphics {
private:
    constexpr const static int anglePerpendicular = 90;
    constexpr const static int degreesTriangle    = 180;
    constexpr const static int degreesRectangle   = 360;
    constexpr const static int degreesCircle      = degreesRectangle;

    constexpr const static float angleRadPerp = M_PI * anglePerpendicular / degreesTriangle;
    constexpr const static float twoPi        = M_PI * 2.0f;

public:
    static void init(int argc, char** argv, int initialWidth, int initialHeight);

    static void drawFilledRoundedRect(vec2 tLeft, vec2 tRight, vec2 bRight, vec2 bLeft, float radOffsFrac = .025f);
    static void drawFilledRect(vec2 tLeft, vec2 tRight, vec2 bRight, vec2 bLeft);
    static void drawFilledCircle(float cx, float cy, float radius, int nSegments, int degrees = 360, int startAngle = 0);
};
