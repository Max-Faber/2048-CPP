#include <main.h>

int main(int argc, char** argv)
{
    MaxLib::Utilities::processArgv(argv);
    initWindow(argc, argv);
    return 0;
}
