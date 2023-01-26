#include <main.h>
#include <torch/torch.h>

int main(int argc, char** argv)
{
    // auto x = torch::randn({2, 2}).to(torch::kMPS);

    Graphics::init(argc, argv, GameRendering::initialWidth, GameRendering::initialHeight);
    GameState::initialize();
    GameRendering::show();
    return 0;
}
