#include <Graphics.h>

void Graphics::init(int argc, char** argv, int initialWidth, int initialHeight)
{
    glutInit(&argc, argv);
    glutInitWindowSize(initialWidth, initialHeight);
}

void Graphics::drawFilledRoundedRect(glm::vec2 tLeft, glm::vec2 tRight, glm::vec2 bRight, glm::vec2 bLeft, float radOffsFrac)
{
    float maxRectLen = std::fmax(
            std::fmax(abs(tLeft.x - tRight.x), abs(tRight.y - bRight.y)),
            std::fmax(abs(bLeft.x - bRight.x), abs(bLeft.y - tLeft.y))
    );
    const float radOffs         = sinf(angleRadPerp) * (radOffsFrac * maxRectLen);
    const int nSegments         = 100;
    int startAngle              = 0;
    const glm::vec2 tLeftOffs   = glm::vec2(tLeft.x  + radOffs, tLeft.y  - radOffs);
    const glm::vec2 tRightOffs  = glm::vec2(tRight.x - radOffs, tRight.y - radOffs);
    const glm::vec2 bRightOffs  = glm::vec2(bRight.x - radOffs, bRight.y + radOffs);
    const glm::vec2 bLeftOffs   = glm::vec2(bLeft.x  + radOffs, bLeft.y  + radOffs);

    drawFilledRect(glm::vec2(tLeftOffs.x,  tLeft.y),  glm::vec2(tRightOffs.x, tRight.y),
                   glm::vec2(bRightOffs.x, bRight.y), glm::vec2(bLeftOffs.x,  bLeft.y));
    drawFilledRect(glm::vec2(tLeft.x,  tLeftOffs.y),  glm::vec2(tRight.x, tRightOffs.y),
                   glm::vec2(bRight.x, bRightOffs.y), glm::vec2(bLeft.x,  bLeftOffs.y));
    drawFilledCircle(tLeftOffs.x,  tLeftOffs.y,  radOffs, nSegments, anglePerpendicular, startAngle += anglePerpendicular);
    drawFilledCircle(bLeftOffs.x,  bLeftOffs.y,  radOffs, nSegments, anglePerpendicular, startAngle += anglePerpendicular);
    drawFilledCircle(bRightOffs.x, bRightOffs.y, radOffs, nSegments, anglePerpendicular, startAngle += anglePerpendicular);
    drawFilledCircle(tRightOffs.x, tRightOffs.y, radOffs, nSegments, anglePerpendicular, startAngle +  anglePerpendicular);
}

void Graphics::drawFilledRect(glm::vec2 tLeft, glm::vec2 tRight, glm::vec2 bRight, glm::vec2 bLeft)
{
    glBegin(GL_QUADS);
    glVertex2f(tLeft.x,  tLeft.y);
    glVertex2f(tRight.x, tRight.y);
    glVertex2f(bRight.x, bRight.y);
    glVertex2f(bLeft.x,  bLeft.y);
    glEnd();
}

void Graphics::drawFilledCircle(float cx, float cy, float radius, int nSegments, int degrees, int startAngle)
{
    const float s         = (float)degrees / degreesCircle;
    const float twoPiFrac = twoPi * s;
    const float radSeg    = twoPiFrac / float(nSegments);
    const float radStart  = (float)startAngle / degreesCircle * (float) twoPi;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy); // Center of circle
    for (int i = 0; i <= nSegments; i++) {
        float theta = radSeg * float(i) + radStart;
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);

        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}
