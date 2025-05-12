#include "../imgsoa/imagesoa.hpp"

#include "gtest/gtest.h"
#include <fstream>
#include <sstream>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace {
  // Function to create a PPM file (3-byte Pixels) for testing.
  void createTestPPMFile3(std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    file << "P6\n2 2\n255\n";  // PPM header for a 2x2 image with max color value of 255.
    file << static_cast<char>(255) << static_cast<char>(0) << static_cast<char>(0);    // Red
    file << static_cast<char>(0) << static_cast<char>(255) << static_cast<char>(0);    // Green
    file << static_cast<char>(0) << static_cast<char>(0) << static_cast<char>(255);    // Blue
    file << static_cast<char>(255) << static_cast<char>(255) << static_cast<char>(0);  // Yellow
    file.close();
  }

  // Function to create a PPM file (6-byte Pixels) for testing.
  void createTestPPMFile6(std::string& filename) {
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

// Tests for readImageSOA function for an image with 3-byte Pixels.
TEST(SOATests, ReadSmallPixelImage) {
  std::string filename = "imageSmall.ppm";
  createTestPPMFile3(filename);
  SOAImage const image = readImageSOA(filename);
  // Expected values for a 2x2 image with max color value of 255
  EXPECT_EQ(image.width, 2);
  EXPECT_EQ(image.height, 2);
  EXPECT_EQ(image.max_color_value, 255);
  // Check the red, green, and blue components are correctly loaded
  std::vector<uint8_t> const expected_red = {255, 0, 0, 255};
  std::vector<uint8_t> const expected_green = {0, 255, 0, 255};
  std::vector<uint8_t> const expected_blue = {0, 0, 255, 0};
  EXPECT_EQ(image.red1_components, expected_red);
  EXPECT_EQ(image.green1_components, expected_green);
  EXPECT_EQ(image.blue1_components, expected_blue);
}

// Tests for readImageSOA function for an image with 6-byte Pixels.
TEST(SOATests, ReadLargePixelImage) {
  std::string filename = "imagelarge.ppm";
  createTestPPMFile6(filename);
  SOAImage const image = readImageSOA(filename);
  // Expected values for a 2x2 image with max color value of 65535
  EXPECT_EQ(image.width, 2);
  EXPECT_EQ(image.height, 2);
  EXPECT_EQ(image.max_color_value, 65535);
  // Check the red, green, and blue components are correctly loaded
  std::vector<uint16_t> const expected_red = {65535, 0, 0,65535};
  std::vector<uint16_t> const expected_green = {0, 65535, 0,65535};
  std::vector<uint16_t> const expected_blue = {0, 0, 65535,0};
  EXPECT_EQ(image.red2_components, expected_red);
  EXPECT_EQ(image.green2_components, expected_green);
  EXPECT_EQ(image.blue2_components, expected_blue);
}


// Tests for writeImageSOA function for an image with 3-bytes Pixels.
TEST(PPMImageTest, WriteImageSmall) {
  SOAImage image = {.width=2, .height=2, .max_color_value=255, .red1_components={},.green1_components={}, .blue1_components={},.red2_components={},.green2_components={},.blue2_components={}};
  image.red1_components = {255,0,0,255};
  image.green1_components = {0,255,0,255};
  image.blue1_components = {0,0,255,0};
  // Write this image in an output file "output_image.ppm".
  const std::string filename = "outputtt_image.ppm";
  writeImageSOA(filename, image);
  // Verify it was written correctly:
  SOAImage writtenImage = readImageSOA(filename);
  EXPECT_EQ(writtenImage.width, image.width);
  EXPECT_EQ(writtenImage.height, image.height);
  EXPECT_EQ(writtenImage.max_color_value, image.max_color_value);
  EXPECT_EQ(writtenImage.red1_components.size(), image.red1_components.size());
  EXPECT_EQ(writtenImage.green1_components.size(), image.green1_components.size());
  EXPECT_EQ(writtenImage.blue1_components.size(), image.blue1_components.size());
  EXPECT_EQ(writtenImage.red1_components[0], image.red1_components[0]); // Check one pixel's red component.
}


// Tests for writeImageSOA function for an image with 6-bytes Pixels.
TEST(PPMImageTest, WriteImageLarge) {
  SOAImage image = {.width=2, .height=2, .max_color_value=65535, .red1_components={},.green1_components={}, .blue1_components={},.red2_components={},.green2_components={},.blue2_components={}};
  image.red2_components = {65535,0,0,65535};
  image.green2_components = {0,65535,0,65535};
  image.blue2_components = {0,0,65535,0};
  // Write this image in an output file "output_image.ppm".
  const std::string filename = "outputt_image.ppm";
  writeImageSOA(filename, image);
  // Verify it was written correctly:
  SOAImage writtenImage = readImageSOA(filename);
  EXPECT_EQ(writtenImage.width, image.width);
  EXPECT_EQ(writtenImage.height, image.height);
  EXPECT_EQ(writtenImage.max_color_value, image.max_color_value);
  EXPECT_EQ(writtenImage.red2_components.size(), image.red2_components.size());
  EXPECT_EQ(writtenImage.green2_components.size(), image.green2_components.size());
  EXPECT_EQ(writtenImage.blue2_components.size(), image.blue2_components.size());
  EXPECT_EQ(writtenImage.red2_components[0], image.red2_components[0]); // Check one pixel's red component.
}



// infoImageSOA function for 3-byte Pixel image.
TEST(PPMImageTest, InfoAOSSmall) {
  std::string filename = "SmallImage.ppm";
  createTestPPMFile3(filename); // Create an image with Small Pixels.
  // Redirect cout to an ostringstream to capture output and be able to compare it with the actual output.
  std::ostringstream const output_capture;
  std::streambuf* original_cout = std::cout.rdbuf();
  std::cout.rdbuf(output_capture.rdbuf());
  infoImageSOA(filename); // Call the function we are testing.
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


// infoImageAOS function for 6-byte Pixel image.
TEST(PPMImageTest, InfoAOSLarge) {
  std::string filename = "LargeImage.ppm";
  createTestPPMFile6(filename); // Create an image with Large Pixels.
  // Redirect cout to an ostringstream to capture output and be able to compare it with the actual output.
  std::ostringstream const output_capture;
  std::streambuf* original_cout = std::cout.rdbuf();
  std::cout.rdbuf(output_capture.rdbuf());
  infoImageSOA(filename); // Call the function we are testing.
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