#include "gtest/gtest.h"
#include <iostream>
#include <stdexcept>
#include "../imgaos/imageaos.hpp"
#include "../imgaos/resizeaos.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cert-env33-c)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

// We define this function to check if a file exists
namespace {

  auto file_exists(const std::string& filename) -> bool {
    struct stat buffer{};
    return (stat(filename.c_str(), &buffer) == 0);
  }

  // Create a PPM files with SmallPixel data for testing:
  void createSmallPixelTestFile(const std::string& filename, int maxColorValue) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n2 2\n" << maxColorValue << "\n";  // The max color value is passed as an argument.
    file << static_cast<char>(maxColorValue) << static_cast<char>(0)
         << static_cast<char>(0);  // Red component = max value, 0, 0.
    file << static_cast<char>(0) << static_cast<char>(maxColorValue)
         << static_cast<char>(0);  // Green component = 0, max value, 0
    file << static_cast<char>(0) << static_cast<char>(0)
         << static_cast<char>(maxColorValue);  // Blue component = 0, 0, max value.
    file << static_cast<char>(maxColorValue) << static_cast<char>(maxColorValue)
         << static_cast<char>(0);  // Yellow component = max value, max value, 0.
    file.close();
  }

  // Create a PPM files with LargePixel data for testing:
  void createLargePixelTestFile(const std::string& filename, int maxColorValue) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n2 2\n" << maxColorValue << "\n";
    // Red pixel: max red, 0 green, 0 blue
    file.write(reinterpret_cast<const char*>(&maxColorValue), 2);  // Red Component = max value.
    file.write("\0\0", 2);                                          // Green Component = 0
    file.write("\0\0", 2);                                          // Blue Component = 0
    // Green pixel: 0 red, max green, 0 blue
    file.write("\0\0", 2);                                          // Red Component = 0.
    file.write(reinterpret_cast<const char*>(&maxColorValue), 2);  // Green Component = max value.
    file.write("\0\0", 2);                                          // Blue Component = 0.
    // Blue pixel: 0 red, 0 green, max blue
    file.write("\0\0", 2);                                          // Red Component = 0.
    file.write("\0\0", 2);                                          // Green Component = 0
    file.write(reinterpret_cast<const char*>(&maxColorValue), 2);  // Blue Component = max value.
    // Yellow pixel: max red, max green, 0 blue
    file.write(reinterpret_cast<const char*>(&maxColorValue), 2);  // Red Component = max value.
    file.write(reinterpret_cast<const char*>(&maxColorValue), 2);  // Green Component = max value.
    file.write("\0\0", 2);                                          // Blue Component = 0.
    file.close();
  }
}

// Test for compress operation for a small image.
TEST(ImtoolAOSTests, CompressOperationSmallImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createSmallPixelTestFile("input.ppm",255);
  std::string const command = executable + " input.ppm output_compressed.cppm compress";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.cppm"));
}


// Test for compress operation for a large image.
TEST(ImtoolAOSTests, CompressOperationLargeImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createLargePixelTestFile(std::string("input.ppm"),65535);
  std::string const command = executable + " input.ppm output_compressed.cppm compress";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.cppm"));
}



// Test for cutfreq operation for a small image.
TEST(ImtoolAOSTests, CutfreqOperationSmallImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createSmallPixelTestFile("input.ppm",254);
  std::string const command = executable + " input.ppm output_compressed.ppm cutfreq 100";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for cutfreq operation for a large image.
TEST(ImtoolAOSTests, CutfreqOperationLargeImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createLargePixelTestFile("input.ppm",65535);
  std::string const command = executable + " input.ppm output_compressed.ppm cutfreq 1000";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for info operation for a small image.
TEST(ImtoolAOSTests, InfoOperationSmallImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createSmallPixelTestFile("inputSmall.ppm",255);
  std::string const command = executable + " inputSmall.ppm output_compressed.ppm info";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for info operation for a large image.
TEST(ImtoolAOSTests, InfoOperationLargeImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createLargePixelTestFile("inputLarge.ppm",65535);
  std::string const command = executable + " inputLarge.ppm output_compressed.ppm info";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for maxlevel operation for a small image.
TEST(ImtoolAOSTests, MaxLevelOperationSmallImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createSmallPixelTestFile("input.ppm",255);
  std::string const command = executable + " input.ppm output_compressed.cppm maxlevel 128";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for maxlevel operation for a large image.
TEST(ImtoolAOSTests, MaxLevelOperationLargeImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createLargePixelTestFile("input.ppm",65535);
  std::string const command = executable + " input.ppm output_compressed.ppm maxlevel 3000";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for resize operation for a small image.
TEST(ImtoolAOSTests, ResizeOperationSmallImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createSmallPixelTestFile("input.ppm",255);
  std::string const command = executable + " input.ppm output_compressed.ppm resize 23 45";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for resize operation for a large image.
TEST(ImtoolAOSTests, ResizeOperationLargeImage) {
  const std::string executable = "./imtool-aos";  // We want to test the imtool-aos executable.
  // Build the command to run the executable with arguments
  createLargePixelTestFile("input.ppm",65535);
  std::string const command = executable + " input.ppm output_compressed.ppm resize 120 90";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-aos with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(cert-env33-c)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

