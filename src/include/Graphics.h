#pragma once

#include <cstdio>
#include <cmath>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>
#include <Controllers/Keyboard.h>

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

    rectPosition operator += (rectPosition const &obj)
    {
        this->tLeft  += obj.tLeft;
        this->tRight += obj.tRight;
        this->bRight += obj.bRight;
        this->bLeft  += obj.bLeft;
        this->center += obj.center;
        return *this;
    }

    rectPosition operator * (glm::vec2 const &obj) const
    {
        rectPosition res(tLeft, tRight, bRight, bLeft, center);

        res.tLeft  *= obj;
        res.tRight *= obj;
        res.bRight *= obj;
        res.bLeft  *= obj;
        res.center *= obj;
        return res;
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
