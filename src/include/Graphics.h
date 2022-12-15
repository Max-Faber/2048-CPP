#pragma once

#include <cstdio>
#include <cmath>
#include <glm/vec2.hpp>
#ifdef __APPLE_CC__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#ifdef _WIN32
    #include <corecrt_math_defines.h>
#endif

struct colorClamp
{
    float R, G, B;
    static const int colorRangeRGB = 255;

    colorClamp(float rClamp, float gClamp, float bClamp)
    {
        this->R = rClamp;
        this->G = gClamp;
        this->B = bClamp;
    }

    colorClamp(int rRGB, int gRGB, int bRGB)
    {
        this->R = (float)rRGB / colorRangeRGB;
        this->G = (float)gRGB / colorRangeRGB;
        this->B = (float)bRGB / colorRangeRGB;
    }
};

struct rectPosition
{
    glm::vec2 tLeft, tRight, bRight, bLeft;

    rectPosition(glm::vec2 tLeft, glm::vec2 tRight, glm::vec2 bRight, glm::vec2 bLeft)
    {
        this->tLeft  = tLeft;
        this->tRight = tRight;
        this->bRight = bRight;
        this->bLeft  = bLeft;
    }
};

class Graphics
{
private:
    constexpr const static int anglePerpendicular = 90;
    constexpr const static int degreesTriangle    = 180;
    constexpr const static int degreesRectangle   = 360;
    constexpr const static int degreesCircle      = degreesRectangle;

    constexpr const static float angleRadPerp = M_PI * anglePerpendicular / degreesTriangle;
    constexpr const static float twoPi        = M_PI * 2.0f;

public:
    static void init(int argc, char** argv, int initialWidth, int initialHeight);

    static void drawFilledRoundedRect(glm::vec2 tLeft, glm::vec2 tRight, glm::vec2 bRight, glm::vec2 bLeft, float radOffsFrac = .025f);
    static void drawFilledRect(glm::vec2 tLeft, glm::vec2 tRight, glm::vec2 bRight, glm::vec2 bLeft);
    static void drawFilledCircle(float cx, float cy, float radius, int nSegments, int degrees = 360, int startAngle = 0);
};
