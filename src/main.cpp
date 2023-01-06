#include <main.h>

int main(int argc, char** argv)
{
    Graphics::init(argc, argv, GameRendering::initialWidth, GameRendering::initialHeight);
    GameState::initialize();
    GameRendering::show();
    return 0;
}
