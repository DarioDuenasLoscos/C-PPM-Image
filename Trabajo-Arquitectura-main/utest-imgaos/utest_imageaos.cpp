#include "gtest/gtest.h"
#include <sstream>
#include <array>
#include "../imgaos/imageaos.hpp"


// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace {
  // Function to create a PPM file (Small Pixels) for testing the readImageAOS function.
  void createTestPPMFileSmall(std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n2 2\n255\n";  // PPM header for a 2x2 image with max color value of 255.
    file << static_cast<char>(255) << static_cast<char>(0) << static_cast<char>(0);    // Red
    file << static_cast<char>(0) << static_cast<char>(255) << static_cast<char>(0);    // Green
    file << static_cast<char>(0) << static_cast<char>(255) << static_cast<char>(0);    // Blue
    file << static_cast<char>(255) << static_cast<char>(255) << static_cast<char>(0);  // Yellow
    file.close();
  }

  // Function to create a PPM file (Large Pixels) for testing the readImageAOS function.
  void createTestPPMFileLarge(std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n2 2\n65535\n";  // PPM header for a 2x2 image with max color value of 65535.
    std::array<std::array<uint16_t, 3>, 4> const pixels = {
        {
         {65535, 0, 0},     // Red
            {0, 65535, 0},     // Green
            {0, 0, 65535},     // Blue
            {65535, 65535, 0}  // Yellow
        }
    };
    for (const auto& pixel : pixels) {
      file.write(reinterpret_cast<const char*>(pixel.data()), sizeof(uint16_t));  // Red Component
      file.write(reinterpret_cast<const char*>(&pixel[1]), sizeof(uint16_t));  // Green Component
      file.write(reinterpret_cast<const char*>(&pixel[2]), sizeof(uint16_t));  // Blue Component
    }
    file.close();
  }
}

// Tests for readImageAOS function for an image with Small Pixels.
TEST(PPMImageTest, ReadValidImageSmall) {
  std::string filename = "imageSmall.ppm";
  createTestPPMFileSmall(filename); // Create a known image
  PPMImageAOS image = readImageAOS(filename);
  EXPECT_EQ(image.width, 2);
  EXPECT_EQ(image.height, 2);
  EXPECT_EQ(image.max_color_value, 255);
  EXPECT_EQ(image.sPixels.size(), 4);
  EXPECT_EQ(image.sPixels[0].red, 255); // Assuming first pixel is red
  EXPECT_EQ(image.sPixels[1].green, 255); // Assuming second pixel is green
}

// Tests for readImageAOS function for an image with Large Pixels.
TEST(PPMImageTest, ReadValidImageLarge) {
  std::string filename = "imageLarge.ppm";
  createTestPPMFileLarge(filename); // Create a known image
  PPMImageAOS image = readImageAOS(filename);
  EXPECT_EQ(image.width, 2);
  EXPECT_EQ(image.height, 2);
  EXPECT_EQ(image.max_color_value, 65535);
  EXPECT_EQ(image.lPixels.size(), 4);
  EXPECT_EQ(image.lPixels[0].red, 65535); // Assuming first pixel is red
  EXPECT_EQ(image.lPixels[1].green, 65535); // Assuming second pixel is green
}

// Tests for writeImageAOS function for an image with Small Pixels.
TEST(PPMImageTest, WriteValidImageSmall) {
  PPMImageAOS image = {.width=2, .height=2, .max_color_value=255, .sPixels={},.lPixels={}};
  image.sPixels = {
    {.red=255, .green=0, .blue=0},   // Red
    {.red=0, .green=255, .blue=0},   // Green
    {.red=0, .green=0, .blue=255},   // Blue
    {.red=255, .green=255, .blue=0}  // Yellow
  };
  // Write this image in an output file "output_image.ppm".
  writeImageAOS("output_image.ppm", image);
  // Verify it was written correctly:
  PPMImageAOS writtenImage = readImageAOS("output_image.ppm");
  EXPECT_EQ(writtenImage.width, image.width);
  EXPECT_EQ(writtenImage.height, image.height);
  EXPECT_EQ(writtenImage.max_color_value, image.max_color_value);
  EXPECT_EQ(writtenImage.sPixels.size(), image.sPixels.size());
  EXPECT_EQ(writtenImage.sPixels[0].red, image.sPixels[0].red); // Check one pixel
}


// Tests for writeImageAOS function for an image with Large Pixels.
TEST(PPMImageTest, WriteValidImageLarge) {
  PPMImageAOS image = {.width=2, .height=2, .max_color_value=65535, .sPixels={},.lPixels={}};
  image.lPixels = {
    {.red=65535, .green=0, .blue=0},   // Red
    {.red=0, .green=65535, .blue=0},   // Green
    {.red=0, .green=0, .blue=65535},   // Blue
    {.red=65535, .green=65535, .blue=0}  // Yellow
  };
  // Write this image in an output file "output_image.ppm".
  writeImageAOS("outputt_image.ppm", image);
  // Verify it was written correctly:
  PPMImageAOS writtenImage = readImageAOS("outputt_image.ppm");
  EXPECT_EQ(writtenImage.width, image.width);
  EXPECT_EQ(writtenImage.height, image.height);
  EXPECT_EQ(writtenImage.max_color_value, image.max_color_value);
  EXPECT_EQ(writtenImage.lPixels.size(), image.lPixels.size());
  EXPECT_EQ(writtenImage.lPixels[0].red, image.lPixels[0].red); // Check one pixel
}

// infoImageAOS function for Small Pixel image.
TEST(PPMImageTest, InfoAOSSmall) {
  std::string filename = "SmallImage.ppm";
  createTestPPMFileSmall(filename); // Create an image with Small Pixels.
  // Redirect cout to an ostringstream to capture output and be able to compare it with the actual output.
  std::ostringstream const output_capture;
  std::streambuf* original_cout = std::cout.rdbuf();
  std::cout.rdbuf(output_capture.rdbuf());
  infoImageAOS(filename); // Call the function we are testing.
  std::cout.rdbuf(original_cout); // Restore original cout.
  // Define the expected output string:
  std::string const expected_output =
      "Metadata for image: SmallImage.ppm\n"
      "--------------------------------\n"
      "Width: 2 px\n"
      "Height: 2 px\n"
      "Max Color Value: 255\n"
      "Pixel Format: 3 bytes per pixel (8-bit color depth)\n";
  EXPECT_EQ(output_capture.str(), expected_output); // Compare actual output with the expected output.
}

// infoImageAOS function for Large Pixel image.
TEST(PPMImageTest, InfoAOSLarge) {
  std::string filename = "LargeImage.ppm";
  createTestPPMFileLarge(filename); // Create an image with Large Pixels.
  // Redirect cout to an ostringstream to capture output and be able to compare it with the actual output.
  std::ostringstream const output_capture;
  std::streambuf* original_cout = std::cout.rdbuf();
  std::cout.rdbuf(output_capture.rdbuf());
  infoImageAOS(filename); // Call the function we are testing.
  std::cout.rdbuf(original_cout); // Restore original cout.
  // Define the expected output string:
  std::string const expected_output =
      "Metadata for image: LargeImage.ppm\n"
      "--------------------------------\n"
      "Width: 2 px\n"
      "Height: 2 px\n"
      "Max Color Value: 65535\n"
      "Pixel Format: 6 bytes per pixel (16-bit color depth)\n";
  EXPECT_EQ(output_capture.str(), expected_output); // Compare actual output with the expected output.
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)