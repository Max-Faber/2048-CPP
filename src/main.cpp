#include <main.h>
//#include <torch/torch.h>

int main(int argc, char** argv)
{
//    torch::TensorOptions tensorOptions = torch::TensorOptions().device(torch::kMPS);
//    torch::Tensor x = torch::randn({2, 2}, tensorOptions);

//    std::cout << x;

    Graphics::init(argc, argv, GameRendering::initialWidth, GameRendering::initialHeight);
    GameState::initialize();
    GameRendering::show();
    return 0;
}
