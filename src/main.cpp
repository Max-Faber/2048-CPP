#include <main.h>

int main(int argc, char** argv) {
    Graphics::init(argc, argv, GameRendering::initialWidth, GameRendering::initialHeight);
    GameLogic::initialize();
    GameRendering::show();
    return 0;
}
