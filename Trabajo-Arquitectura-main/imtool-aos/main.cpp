#include "../common/progargs.hpp"
#include "../imgaos/imageaos.hpp"
#include <iostream>
#include <string>

auto main(int argc, char *argv[]) -> int {
    try {
        std::vector <std::string> const argsVector(argv, argv + argc); // Create a string vector for program arguments.
        ProgramArgs const args = parseArgs(argsVector);
        run_operationaos(args);
        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
