#ifndef PROGARGS_HPP
#define PROGARGS_HPP

#include <vector>
#include <string>

// Structure to store the parameters for the program.
struct ProgramArgs {
    std::string input_file;
    std::string output_file;
    std::string operation;
    int max_level = -1; // -1 by default, meaning not defined yet.
    int width = -1; // Width for the resize.
    int height = -1; // Height for the resize.
};

struct OperationData {
    std::string operation;
    std::vector<std::string> argsvector;
    int index;
};

auto parseArgs(const std::vector <std::string> &argsVector) -> ProgramArgs; // Parses and validates the arguments passed to the program.

void printErrorAndExit(const std::string &message); // Prints an error message and exits the program.

void printExtraArgumentsError(const OperationData &data); // Prints an error for extra arguments in a specific operation and exits.

void validateArgsExtra(const OperationData &data); // Validates if the argument count matches expected value for the given operation.

void validateArgsCount(const OperationData &data);// Checks if there are unexpected extra arguments and prints an error if so.

void validateMaxLevel(const std::vector <std::string> &argsVector,
                      ProgramArgs &args); // Validates the "maxlevel" operation arguments, ensuring max level is within allowed range.

void validateResize(const std::vector <std::string> &argsVector,
                    ProgramArgs &args); // Validates the "resize" operation arguments, checking width and height are valid.

void validateCutFreq(const std::vector <std::string> &argsVector,
                     ProgramArgs &args); // Validates the "cutfreq" operation arguments, ensuring valid max level for frequency cutoff.


#endif // PROGARGS_HPP
