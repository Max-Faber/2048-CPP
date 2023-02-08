#include <main.h>
#include <torch/torch.h>

int main(int argc, char** argv)
{
//    torch::TensorOptions tensorOptions = torch::TensorOptions().device(torch::kMPS);
    torch::TensorOptions tensorOptions = torch::TensorOptions().device(torch::kCPU);
    torch::Tensor x = torch::randn({2, 2}, tensorOptions);

    std::cout << x << "\n";
    Graphics::init(argc, argv, GameRendering::initialWidth, GameRendering::initialHeight);
    GameState::initialize();
    GameRendering::show();
    return 0;
}
