#include <main.h>

int main(int argc, char** argv) {
    Graphics::init(argc, argv, GameDrawing::initialWidth, GameDrawing::initialHeight);
    GameLogic::initialize();
    GameDrawing::show();
    return 0;
}
