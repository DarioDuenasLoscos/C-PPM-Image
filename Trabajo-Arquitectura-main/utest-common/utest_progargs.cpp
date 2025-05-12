#include "gtest/gtest.h"
#include "../common/progargs.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

// Minimum required arguments
TEST(ProgArgsTest, MinArguments) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "info"};
  ProgramArgs const parsedArgs = parseArgs(args);
  EXPECT_EQ(parsedArgs.input_file, "input.ppm");
  EXPECT_EQ(parsedArgs.output_file, "output.ppm");
  EXPECT_EQ(parsedArgs.operation, "info");
}

// Number of arguments not valid: input file not passed
TEST(ProgArgsTest, ArgumentsNotValid0) {
  std::vector<std::string> const args = {"program"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of arguments: 0");
}

// Number of arguments not valid: output file not passed
TEST(ProgArgsTest, ArgumentsNotValid1) {
  std::vector<std::string> const args = {"program", "input.ppm"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of arguments: 1");
}

// Number of arguments not valid: operation not passed
TEST(ProgArgsTest, ArgumentsNotValid2) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of arguments: 2");
}

// Invalid operation
TEST(ProgArgsTest, UnsupportedOperation) {
  std::vector<std::string> const args = {"program","input.ppm", "output.ppm", "invalid_op"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Unsupported operation: invalid_op");
}


// Info operation with extra arguments
TEST(ProgArgsTest, InfoExtraArgs) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "info", "extra_arg"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid extra arguments for info: extra_arg");
}


// MaxLevel operation with missing arguments
TEST(ProgArgsTest, MaxLevelMissingArgs0) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "maxlevel"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of extra arguments for maxlevel: 0");
}

// MaxLevel operation with extra arguments
TEST(ProgArgsTest, MaxLevelExtraArgs2) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "maxlevel", "4000","1000"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of extra arguments for maxlevel: 2");
}


// Maxlevel with valid maxlevel
TEST(ProgArgsTest, MaxLevel) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "maxlevel", "100"};
  ProgramArgs const parsedArgs = parseArgs(args);
  EXPECT_EQ(parsedArgs.max_level, 100);
}

// Invalid MaxLevel
TEST(ProgArgsTest, InvalidMaxLevel) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "maxlevel", "-500"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid maxlevel: -500");
}


// Resize operation with missing arguments
TEST(ProgArgsTest, ResizeMissingArgs0) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "resize"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of extra arguments for resize: 0");
}

// Resize operation with missing arguments
TEST(ProgArgsTest, ResizeMissingArgs1) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "resize", "1200"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of extra arguments for resize: 1");
}

// Resize operation with extra arguments
TEST(ProgArgsTest, ResizeExtraArgs3) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "resize", "1200","2000","3450"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of extra arguments for resize: 3");
}

// Resize with valid width and height
TEST(ProgArgsTest, Resize) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "resize", "640", "480"};
  ProgramArgs const parsedArgs = parseArgs(args);
  EXPECT_EQ(parsedArgs.width, 640);
  EXPECT_EQ(parsedArgs.height, 480);
}

// Resize with invalid width
TEST(ProgArgsTest, ResizeInvalidWidth) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "resize", "-10", "50"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid resize width: -10");
}

// Resize with invalid height
TEST(ProgArgsTest, ResizeInvalidHeight) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "resize", "10", "-50"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid resize height: -50");
}


// Resize operation with missing arguments
TEST(ProgArgsTest, CutfreqMissingArgs0) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "cutfreq"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of extra arguments for cutfreq: 0");
}

// Resize operation with extra arguments
TEST(ProgArgsTest, CutfreqExtraArgs2) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "cutfreq", "1200", "500"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255), "Invalid number of extra arguments for cutfreq: 2");
}


// Cutfreq with valid cutfreq level
TEST(ProgArgsTest, CutFreq) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "cutfreq", "50"};
  ProgramArgs const parsedArgs = parseArgs(args);
  EXPECT_EQ(parsedArgs.max_level, 50);
}

// Cutfreq with invalid cutfreq level
TEST(ProgArgsTest, InvalidCutFreq) {
  std::vector<std::string> const args = {"program", "input.ppm", "output.ppm", "cutfreq", "-1"};
  EXPECT_EXIT(parseArgs(args), ::testing::ExitedWithCode(255),"Invalid cutfreq: -1");
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
