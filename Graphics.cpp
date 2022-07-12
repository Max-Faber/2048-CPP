#include <Graphics.h>

void Graphics::init(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(INITIAL_WIDTH, INITIAL_HEIGHT);

};

void Graphics::show() {
    glutCreateWindow("2048-C++");
    glutDisplayFunc(Graphics::display);
    glutMainLoop();
};

void Graphics::display() {
    int width = glutGet(GLUT_WINDOW_WIDTH), height = glutGet(GLUT_WINDOW_HEIGHT);
    float aspect_ratio = (float)width / (float)height;

    // Set every pixel in the frame buffer to the current clear color.
    glClearColor(Graphics::backgroundColor, Graphics::backgroundColor, Graphics::backgroundColor, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-aspect_ratio, aspect_ratio, -1, 1);

    // Drawing is done by specifying a sequence of vertices.  The way these
    // vertices are connected (or not connected) depends on the argument to
    // glBegin.  GL_POLYGON constructs a filled polygon.
    glBegin(GL_QUADS);
    glColor3f(Graphics::gridBackgroundColor[0], Graphics::gridBackgroundColor[1], Graphics::gridBackgroundColor[2]);
    glVertex2f(-.5f, -.5f);  // Top left
    glVertex2f(.5f, -.5f);  // Top right
    glVertex2f(.5f, .5f);   // Bottom right
    glVertex2f(-.5f, .5f);   // Bottom left
    glEnd();
    // Flush drawing command buffer to make drawing happen as soon as possible.
    glFlush();
};
