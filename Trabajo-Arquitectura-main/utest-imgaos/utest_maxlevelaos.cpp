#include "gtest/gtest.h"
#include "../imgaos/maxlevelaos.hpp"
#include "../common/binaryio.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace {
  // Create a PPM files with SmallPixel data for testing:
  void createSmallPixelTestFile(std::string& filename) {
    const int maxColorValue = 255;
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
  void createLargePixelTestFile(std::string& filename) {
    const int maxColorValue = 65335;
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
// Test scaleSmallToLarge
TEST(PPMImageTest, ScaleSmallPixelToLarge) {
    std::string filename = "small_to_large_test.ppm";
    createSmallPixelTestFile(filename); // Create an initial small pixel image with maxColorValue 255.
    PPMImageAOS scaledImage = maxLevelImageAOS(filename, 65535); // New max level value = 65535.
    ASSERT_EQ(scaledImage.width, 2);
    ASSERT_EQ(scaledImage.height, 2);
    ASSERT_EQ(scaledImage.max_color_value, 65535);
    ASSERT_EQ(scaledImage.lPixels.size(), 4);
  // Check the first pixel of scaled image (should be red)
    EXPECT_EQ(scaledImage.lPixels[0].red, 65535);
    EXPECT_EQ(scaledImage.lPixels[0].green, 0);
    EXPECT_EQ(scaledImage.lPixels[0].blue, 0);
}


// Test scaleLargeToSmall
TEST(PPMImageTest, ScaleLargePixelToSmall) {
    std::string filename = "large_to_small_test.ppm";
    createLargePixelTestFile(filename); // Create an initial large pixel image with maxColorValue 65535.
    PPMImageAOS scaledImage = maxLevelImageAOS(filename, 255); // New max level value = 255.
    ASSERT_EQ(scaledImage.width, 2);
    ASSERT_EQ(scaledImage.height, 2);
    ASSERT_EQ(scaledImage.max_color_value, 255);
    ASSERT_EQ(scaledImage.sPixels.size(), 4);
    // Check the third pixel of scaled image (should be blue)
    EXPECT_EQ(scaledImage.sPixels[2].red, 0);
    EXPECT_EQ(scaledImage.sPixels[2].green, 0);
    EXPECT_EQ(scaledImage.sPixels[2].blue, 255);
}


// Test scaleSmallToSmall
TEST(PPMImageTest, ScaleSmallPixelToSmall) {
    std::string filename = "small_to_small_test.ppm";
    createSmallPixelTestFile(filename);
    PPMImageAOS scaledImage = maxLevelImageAOS(filename, 128); // New max level value = 128.
    ASSERT_EQ(scaledImage.width, 2);
    ASSERT_EQ(scaledImage.height, 2);
    ASSERT_EQ(scaledImage.max_color_value, 128);
    ASSERT_EQ(scaledImage.sPixels.size(), 4);
    // Check the second pixel of scaled image (should be green)
    EXPECT_EQ(scaledImage.sPixels[1].red, 0);
    EXPECT_EQ(scaledImage.sPixels[1].green, 128);
    EXPECT_EQ(scaledImage.sPixels[1].blue, 0);
}


// Test scaleLargeToLarge
TEST(PPMImageTest, ScaleLargePixelToLarge) {
    std::string filename = "large_to_large_test.ppm";
    createLargePixelTestFile(filename);
    PPMImageAOS scaledImage = maxLevelImageAOS(filename, 32768); // New max level value = 32768.
    ASSERT_EQ(scaledImage.width, 2);
    ASSERT_EQ(scaledImage.height, 2);
    ASSERT_EQ(scaledImage.max_color_value, 32768);
    ASSERT_EQ(scaledImage.lPixels.size(), 4);
  // Check the fourth pixel of scaled image (should be yellow)
    EXPECT_EQ(scaledImage.lPixels[3].red, 32768);
    EXPECT_EQ(scaledImage.lPixels[3].green, 32768);
    EXPECT_EQ(scaledImage.lPixels[3].blue, 0);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
