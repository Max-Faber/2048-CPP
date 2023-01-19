#pragma once

#include <cstdio>
#include <cmath>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>
#include <InputControl.h>

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
    glm::vec2 tLeft, tRight, bRight, bLeft, center;

    rectPosition(glm::vec2 tLeft, glm::vec2 tRight, glm::vec2 bRight, glm::vec2 bLeft, glm::vec2 center)
    {
        this->tLeft  = tLeft;
        this->tRight = tRight;
        this->bRight = bRight;
        this->bLeft  = bLeft;
        this->center = center;
    }

    rectPosition operator - (rectPosition const &obj) const
    {
        rectPosition res(tLeft, tRight, bRight, bLeft, center);

        res.tLeft  -= obj.tLeft;
        res.tRight -= obj.tRight;
        res.bRight -= obj.bRight;
        res.bLeft  -= obj.bLeft;
        res.center -= obj.center;
        return res;
    }

    bool operator == (rectPosition const &obj) const
    {
        bool equal = true;

        equal &= tLeft  == obj.tLeft;
        equal &= tRight == obj.tRight;
        equal &= bRight == obj.bRight;
        equal &= bLeft  == obj.bLeft;
        equal &= center == obj.center;
        return equal;
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
    static GLFWwindow* window;
    static void init(int argc, char** argv, int initialWidth, int initialHeight);

    static void drawFilledRoundedRect(glm::vec2 tLeft, glm::vec2 tRight, glm::vec2 bRight, glm::vec2 bLeft, float radOffsFrac = .025f);
    static void drawFilledRect(glm::vec2 tLeft, glm::vec2 tRight, glm::vec2 bRight, glm::vec2 bLeft);
    static void drawFilledCircle(float cx, float cy, float radius, int nSegments, int degrees = 360, int startAngle = 0);
};
