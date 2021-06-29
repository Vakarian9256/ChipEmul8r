#include <iostream>
#include <filesystem>
#include <string>
#include "chip8.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
        std::cerr << "[Usage]:" << argv[0] << " <ROM file>." << std::endl;
        exit(EXIT_FAILURE);
	}
    else if(!std::filesystem::exists(std::string(argv[1])))
    {
        std::cerr << "[Path]:" << argv[1] << " does not exist." << std::endl;
        exit(EXIT_FAILURE);
    }
    Chip8 vm(argv[1]);
    vm.run();
    return 0;
}

