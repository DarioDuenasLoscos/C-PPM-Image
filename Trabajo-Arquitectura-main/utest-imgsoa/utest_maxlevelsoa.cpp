#include "gtest/gtest.h"
#include <fstream>
#include "../imgsoa/maxlevelsoa.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace {
  // Function to create a PPM file (3-byte Pixels) for testing.
  void createTestPPMFile3(std::string& filename){
    const int maxColorValue = 255;
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
  void createTestPPMFile6(std::string& filename) {
    const int maxColorValue = 65335;
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

// resizeAndRecalculateToOneByte test
TEST(MaxLevelImageSOATests, ValidConversionTo1Byte) {
  std::string filename = "test_maxlevelsoa.ppm";
  createTestPPMFile3(filename); // 2x2 SOA image with max color value 255.
  SOAImage const inputImage = readImageSOA(filename);
  int const newMaxLevel = 127; // New max level value = 127.
  SOAImage outputImage = maxLevelImageSOA(inputImage, newMaxLevel);
  // Check properties of the new image
  EXPECT_EQ(outputImage.width, inputImage.width);
  EXPECT_EQ(outputImage.height, inputImage.height);
  EXPECT_EQ(outputImage.max_color_value, newMaxLevel);
  // Validate resized components for 1 byte per component
  EXPECT_EQ(outputImage.red1_components[0], 127);
  EXPECT_EQ(outputImage.red1_components[1], 0);
  EXPECT_EQ(outputImage.red1_components[2], 0);
  EXPECT_EQ(outputImage.red1_components[3], 127);
  EXPECT_EQ(outputImage.green1_components[0], 0);
  EXPECT_EQ(outputImage.green1_components[1], 127);
  EXPECT_EQ(outputImage.green1_components[2], 0);
  EXPECT_EQ(outputImage.green1_components[3], 127);

  EXPECT_EQ(outputImage.blue1_components[0], 0);
  EXPECT_EQ(outputImage.blue1_components[1], 0);
  EXPECT_EQ(outputImage.blue1_components[2], 127);
  EXPECT_EQ(outputImage.blue1_components[3], 0);
}


// resizeAndRecalculateToTwoByte test
TEST(MaxLevelImageSOATests, ValidConversionTo2Byte) {
  std::string filename = "test_maxlevelsoa2.ppm";
  createTestPPMFile6(filename); // 2x2 SOA image with max color value 65535.
  SOAImage const inputImage = readImageSOA(filename);
  int const newMaxLevel = 32767; // New max level value = 127.
  SOAImage outputImage = maxLevelImageSOA(inputImage, newMaxLevel);
  // Check properties of the new image
  EXPECT_EQ(outputImage.width, inputImage.width);
  EXPECT_EQ(outputImage.height, inputImage.height);
  EXPECT_EQ(outputImage.max_color_value, newMaxLevel);
  // Validate resized components for 2 bytes per component
  EXPECT_EQ(outputImage.red2_components[0], 32767);
  EXPECT_EQ(outputImage.red2_components[1], 0);
  EXPECT_EQ(outputImage.red2_components[2], 0);
  EXPECT_EQ(outputImage.red2_components[3], 32767);
  EXPECT_EQ(outputImage.green2_components[0], 0);
  EXPECT_EQ(outputImage.green2_components[1], 32767);
  EXPECT_EQ(outputImage.green2_components[2], 0);
  EXPECT_EQ(outputImage.green2_components[3], 32767);
  EXPECT_EQ(outputImage.blue2_components[0], 0);
  EXPECT_EQ(outputImage.blue2_components[1], 0);
  EXPECT_EQ(outputImage.blue2_components[2], 32767);
  EXPECT_EQ(outputImage.blue2_components[3], 0);
}


// Test maxLevelImageSOA function for a 3-byte image.
TEST(SOATests, ImageToLowerMaxLevel3) {
  std::string filename = "test_maxlevelsoa3.ppm";
  createTestPPMFile3(filename);
  SOAImage const image = readImageSOA(filename);
  int const newMaxLevel = 127;
  SOAImage newImage = maxLevelImageSOA(image, newMaxLevel);
  EXPECT_EQ(newImage.width, image.width);
  EXPECT_EQ(newImage.height, image.height);
  EXPECT_EQ(newImage.max_color_value, newMaxLevel);
  EXPECT_EQ(newImage.red1_components.size(), image.red1_components.size());
  // Verify values are recalculated to fit the new max level
  EXPECT_EQ(newImage.red1_components[0], 127);
  EXPECT_EQ(newImage.green1_components[1], 127);
}

// Test for converting a 2-byte component image to a lower max value (32767)
TEST(SOATests, ToLowerMaxLevel6) {
  std::string filename = "test_maxlevelsoa6.ppm";
  createTestPPMFile6(filename);
  SOAImage const image = readImageSOA(filename);
  int const newMaxLevel = 32767;
  SOAImage newImage = maxLevelImageSOA(image, newMaxLevel);
  EXPECT_EQ(newImage.width, image.width);
  EXPECT_EQ(newImage.height, image.height);
  EXPECT_EQ(newImage.max_color_value, newMaxLevel);
  EXPECT_EQ(newImage.red2_components.size(), image.red2_components.size());
  // Verify values are recalculated to fit the new max level
  EXPECT_EQ(newImage.red2_components[0], 32767);
  EXPECT_EQ(newImage.green2_components[1], 32767);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)