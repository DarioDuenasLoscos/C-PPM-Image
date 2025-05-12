#include "progargs.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// Define constants for error codes and argument requirements
static const int ERROR_CODE = -1;                   // Error code for general errors
static const int MIN_ARGS_REQUIRED = 4;             // Minimum number of arguments required
static const int MAX_LEVEL_CUT_FREQ_WIDTH_INDEX = 4; // Maxlevel/cutfreq/width argument index
static const int HEIGHT_INDEX = 4;                   // Height argument index
static const int ARGS_REQUIRED_MAXLEVEL_CUTFREQ = 5;        // Arguments required for "maxlevel" and "cutfreq"
static const int ARGS_REQUIRED_RESIZE = 6;          // Arguments required for "resize"
static const int MAX_LEVEL_UPPER_LIMIT = 65535;     // Upper limit for max level validation



auto parseArgs(const std::vector <std::string> &argsVector) -> ProgramArgs {
    if (argsVector.size() < MIN_ARGS_REQUIRED) { // Check if args are fewer than required
        printErrorAndExit("Invalid number of arguments: " + std::to_string(argsVector.size() - 1));
    }
    ProgramArgs args;
    args.input_file = argsVector[1]; // Set input file
    args.output_file = argsVector[2]; // Set output file
    args.operation = argsVector[3]; // Set operation type

    // Validate based on operation
    if (args.operation == "info" || args.operation == "compress") {
        OperationData const data = {.operation=args.operation, .argsvector=argsVector, .index=MIN_ARGS_REQUIRED};
        validateArgsExtra(data); // For info or compress
    } else if (args.operation == "maxlevel") {
        validateMaxLevel(argsVector, args); // For maxlevel operation
    } else if (args.operation == "resize") {
        validateResize(argsVector, args); // For resize operation
    } else if (args.operation == "cutfreq") {
        validateCutFreq(argsVector, args); // For cutfreq operation
    } else {
        printErrorAndExit("Unsupported operation: " + args.operation); // Unsupported operation
    }

    return args; // Return parsed arguments
}

void printErrorAndExit(const std::string &message) {
  std::cerr << "Error: " << message << "\n";
  std::exit(ERROR_CODE); // Exit with error code
}

void printExtraArgumentsError(const OperationData &data) {
  std::cerr << "Error: Invalid extra arguments for " << data.operation << ": ";
  for (auto i = static_cast<size_t>(data.index); i < data.argsvector.size(); ++i) { // Iterate over extra arguments
    std::cerr << data.argsvector[i] << " ";
  }
  std::cerr << "\n";
  std::exit(ERROR_CODE); // Exit with error code
}

void validateArgsExtra(const OperationData &data) {
  if (data.argsvector.size() != static_cast<size_t>(data.index)) {
    printExtraArgumentsError(data);
  }
}

void validateArgsCount(const OperationData &data) {
  int const extraArgs = static_cast<int>(data.argsvector.size()) - MIN_ARGS_REQUIRED; // Calculate extra arguments
  if (extraArgs != 0) {
    printErrorAndExit("Invalid number of extra arguments for " + data.operation + ": " +
                      std::to_string(extraArgs)); // Print error with extra args count
  } else {
    printErrorAndExit("Invalid number of extra arguments for " + data.operation + ": 0"); // Explicit zero case
  }
}

void validateMaxLevel(const std::vector<std::string> &argsvector, ProgramArgs &args) {
  if (argsvector.size() != ARGS_REQUIRED_MAXLEVEL_CUTFREQ) { // Validate args for maxlevel
    OperationData const data = {.operation="maxlevel", .argsvector=argsvector, .index=MIN_ARGS_REQUIRED};
    validateArgsCount(data);
  }
  try {
    args.max_level = std::stoi(argsvector[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX]); // Parse max level from args
    if (args.max_level <= 0 || args.max_level > MAX_LEVEL_UPPER_LIMIT) { // Validate max level range
      printErrorAndExit("Invalid maxlevel: " + std::to_string(args.max_level)); // Exit if out of range
    }
  } catch (const std::invalid_argument &) {
    printErrorAndExit("Invalid maxlevel: " + argsvector[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX]); // Catch invalid argument
  } catch (const std::out_of_range &) {
    printErrorAndExit("Invalid maxlevel: " + argsvector[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX]); // Catch out of range error
  }
}

void validateResize(const std::vector<std::string> &argv, ProgramArgs &args) {
  if (argv.size() != ARGS_REQUIRED_RESIZE) { // Validate args for resize
    OperationData const data = {.operation="resize", .argsvector=argv, .index=MIN_ARGS_REQUIRED};
    validateArgsCount(data);
  }
  try {
    args.width = std::stoi(argv[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX]); // Parse width
    if (args.width <= 0) {
      printErrorAndExit("Invalid resize width: " + std::to_string(args.width)); // Exit if width invalid
    }
    args.height = std::stoi(argv[HEIGHT_INDEX]); // Parse height
    if (args.height <= 0) {
      printErrorAndExit("Invalid resize height: " + std::to_string(args.height)); // Exit if height invalid
    }
  } catch (const std::invalid_argument &) {
    printErrorAndExit("Invalid resize parameters: " + argv[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX] + ", " + argv[HEIGHT_INDEX]); // Catch invalid argument
  } catch (const std::out_of_range &) {
    printErrorAndExit("Invalid resize parameters: " + argv[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX] + ", " + argv[HEIGHT_INDEX]); // Catch out of range error
  }
}

void validateCutFreq(const std::vector<std::string> &argv, ProgramArgs &args) {
  if (argv.size() != ARGS_REQUIRED_MAXLEVEL_CUTFREQ) { // Validate args for cutfreq
    OperationData const data = {.operation="cutfreq", .argsvector=argv, .index=MIN_ARGS_REQUIRED};
    validateArgsCount(data);
  }
  try {
    args.max_level = std::stoi(argv[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX]); // Parse max level for cutfreq
    if (args.max_level <= 0) {
      printErrorAndExit("Invalid cutfreq: " + std::to_string(args.max_level)); // Exit if cutfreq invalid
    }
  } catch (const std::invalid_argument &) {
    printErrorAndExit("Invalid cutfreq: " + argv[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX]); // Catch invalid argument
  } catch (const std::out_of_range &) {
    printErrorAndExit("Invalid cutfreq: " + argv[MAX_LEVEL_CUT_FREQ_WIDTH_INDEX]); // Catch out of range error
  }
}
