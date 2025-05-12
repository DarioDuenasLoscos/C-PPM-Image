#include "gtest/gtest.h"
#include <iostream>
#include <stdexcept>
#include "../imgsoa/compresssoa.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cert-env33-c)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace {
  // We define this function to check if a file exists
  auto file_exists(const std::string& filename) -> bool {
    struct stat buffer{};
    return (stat(filename.c_str(), &buffer) == 0);
  }

  // Function to create a PPM file (3-byte Pixels) for testing.
  void createTestPPMFile3(const std::string& filename, int maxColorValue) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n2 2\n"
         << maxColorValue << "\n";  // PPM header for a 2x2 image with max color value of 255.
    file << static_cast<char>(maxColorValue) << static_cast<char>(0)
         << static_cast<char>(0);  // Red Pixel
    file << static_cast<char>(0) << static_cast<char>(maxColorValue)
         << static_cast<char>(0);  // Green Pixel
    file << static_cast<char>(0) << static_cast<char>(0)
         << static_cast<char>(maxColorValue);  // Blue Pixel
    file << static_cast<char>(maxColorValue) << static_cast<char>(maxColorValue)
         << static_cast<char>(0);  // Yellow Pixel
    file.close();
  }

  // Function to create a PPM file (6-byte Pixels) for testing.
  void createTestPPMFile6(const std::string& filename, int maxColorValue) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n2 2\n"
         << maxColorValue << "\n";  // PPM header for a 2x2 image with max color value of 65535.
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
TEST(ImtoolSOATests, CompressOperationSmallImage) {
    const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile3("input.ppm",254);
  std::string const command = executable + " input.ppm output_compressed.cppm compress";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.cppm"));
}


// Test for compress operation for a large image.
TEST(ImtoolSOATests, CompressOperationLargeImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile6("input.ppm",65535);
  std::string const command = executable + " input.ppm output_compressed.cppm compress";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.cppm"));
}



// Test for cutfreq operation for a small image.
TEST(ImtoolSOATests, CutfreqOperationSmallImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile3("input.ppm",255);
  std::string const command = executable + " input.ppm output_compressed.ppm cutfreq 100";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for cutfreq operation for a large image.
TEST(ImtoolSOATests, CutfreqOperationLargeImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile6("input.ppm",65535);
  std::string const command = executable + " input.ppm output_compressed.ppm cutfreq 1000";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for info operation for a small image.
TEST(ImtoolSOATests, InfoOperationSmallImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile3("inputSmall.ppm",255);
  std::string const command = executable + " inputSmall.ppm output_compressed.ppm info";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for info operation for a large image.
TEST(ImtoolSOATests, InfoOperationLargeImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile6("inputLarge.ppm",65535);
  std::string const command = executable + " inputLarge.ppm output_compressed.ppm info";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for maxlevel operation for a small image.
TEST(ImtoolSOATests, MaxLevelOperationSmallImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile3("input.ppm",255);
  std::string const command = executable + " input.ppm output_compressed.cppm maxlevel 128";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for maxlevel operation for a large image.
TEST(ImtoolSOATests, MaxLevelOperationLargeImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile6("input.ppm",65535);
  std::string const command = executable + " input.ppm output_compressed.ppm maxlevel 3000";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for resize operation for a small image.
TEST(ImtoolSOATests, ResizeOperationSmallImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile3("input.ppm",255);
  std::string const command = executable + " input.ppm output_compressed.ppm resize 23 45";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}


// Test for resize operation for a large image.
TEST(ImtoolSOATests, ResizeOperationLargeImage) {
  const std::string executable = "./imtool-soa";  // We want to test the imtool-soa executable.
  // Build the command to run the executable with arguments
  createTestPPMFile6("input.ppm",65552);
  std::string const command = executable + " input.ppm output_compressed.ppm resize 120 90";

  // We use std::system() to pass the arguments from the command line to the main.
  int const result = std::system(command.c_str());  // Executes imtool-soa with arguments

  // Check if the program ran successfully (return code 0)
  EXPECT_EQ(result, 0);  // 0 indicates success

  // Check if the output file was created
  EXPECT_TRUE(file_exists("output_compressed.ppm"));
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(cert-env33-c)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

