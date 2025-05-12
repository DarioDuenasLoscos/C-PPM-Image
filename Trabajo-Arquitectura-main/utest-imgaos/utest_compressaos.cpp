#include "gtest/gtest.h"
#include <sstream>
#include "../imgaos/compressaos.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTBEGIN(cppcoreguidelines-avoid-adjacent-parameters)

namespace {

  // We create a function to create PPMImageAOS structures for 3 bytes pixel to be able to perform the tests. The nolint is beacuse the parameters are always the same in the test, but they could be different
  //NOLINTNEXTLINE
  auto createAOSSmallImage() -> PPMImageAOS {
    PPMImageAOS Smallimage;
    Smallimage.width           = 2;
    Smallimage.height          = 2;
    Smallimage.max_color_value = 255;
    Smallimage.sPixels         = {
        {.red=255,   .green=0,   .blue=0},
        {  .red=0, .green=255,   .blue=0},
        {  .red=0,   .green=0, .blue=255},
        {.red=255, .green=255, .blue=255}
    };  // 4 pixels
    return Smallimage;
  }

  // We create a function to create PPMImageAOS structures for 6 bytes pixel to be able to perform the tests.
  //NOLINTNEXTLINE
  auto createAOSLargeImage() -> PPMImageAOS {
    PPMImageAOS Largeimage;
    Largeimage.width           = 2;
    Largeimage.height          = 2;
    Largeimage.max_color_value = 65535;
    Largeimage.lPixels         = {
        {.red=65535,     .green=0,     .blue=0},
        {    .red=0, .green=65535,     .blue=0},
        {    .red=0,     .green=0, .blue=65535},
        {.red=65535, .green=65535, .blue=65535}
    };  // 4 pixels.
    return Largeimage;
  }
}


// Test for write_header function: verify that the header of the output file is written correctly.
TEST(CompressAOSTest, WriteHeaderSmall) {
  PPMImageAOS const image = createAOSSmallImage(); // Create an instance of PPMImageAOS for testing.
  std::ostringstream output; // Output file.
  write_header(output, image, 4);
  std::string const expected = "C6 2 2 255 4\n"; // Expected header format
  EXPECT_EQ(output.str(), expected); // Compare the actual header format with the expected.
}


// Test for write_header function: verify that the header of the output file is written correctly.
TEST(CompressAOSTest, WriteHeaderLarge) {
  PPMImageAOS const image = createAOSLargeImage(); // Create an instance of PPMImageAOS with Large Pixels for testing.
  std::ostringstream output; // Output file.
  write_header(output, image, 4);
  std::string const expected = "C6 2 2 65535 4\n"; // Expected header format
  EXPECT_EQ(output.str(), expected); // Compare the actual header format with the expected.
}


// Test for write_color_table function with Small Pixels: verify the size of the output after writing the color table.
TEST(CompressAOSTest, WriteColorTableSmall) {
  std::ostringstream output; // Output file.
  // Vector of colors (each color component is 1 byte)
  std::vector<SmallPixel> const colors = { {.red=255, .green=0, .blue=0}, {.red=0, .green=255, .blue=0}, {.red=0, .green=0, .blue=255}};
  write_color_table(output, colors);
  EXPECT_EQ(output.str().size(), colors.size() * 3); // Compare the expected and actual output size (3 bytes per pixel).
}


// Test for write_color_table function with Large Pixels: verify the size of the output after writing the color table.
TEST(CompressAOSTest, WriteColorTableLarge) {
  // Vector of colors (each color component is 2 bytes)
  std::vector<LargePixel> const colors = { {.red=65535, .green=0, .blue=0}, {.red=0, .green=65535, .blue=0}, {.red=0, .green=0, .blue=65535}};
  std::ostringstream output; // Output file.
  write_color_table(output, colors);
  EXPECT_EQ(output.str().size(), colors.size()*6); // Compare the expected and actual output size (6 bytes per pixel).
}


// Test for generate_color_table function with Small Pixels: verify that a correct number of unique colors are generated and mapped.
TEST(CompressAOSTest, GenerateColorTableSmall) {
  std::vector<SmallPixel> unique_colors; // holds unique colors identified by the generate_color_table function.
  PPMImageAOS image = createAOSSmallImage(); // Create an instance of PPMImageAOS for testing.
  auto color_map = generate_color_table(image.sPixels, unique_colors);
  EXPECT_EQ(unique_colors.size(), 4); // Should have 4 unique colors (by definition of our PPMImageAOS above).
  EXPECT_EQ(color_map[image.sPixels[0]], 0); // First color should correspond to index 0.
  EXPECT_EQ(color_map[image.sPixels[1]], 1); // Second color should correspond to index 1.
}


// Test for generate_color_table function with Large Pixels: verify that a correct number of unique colors are generated and mapped.
TEST(CompressAOSTest, GenerateColorTableLarge) {
  std::vector<LargePixel> unique_colors;
  PPMImageAOS image = createAOSLargeImage(); // Create an instance of PPMImageAOS with Large Pixels for testing.
  auto color_map = generate_color_table(image.lPixels, unique_colors);
  EXPECT_EQ(unique_colors.size(), 4); // Should have 4 unique colors (by definition of our PPMImageAOS above).
  EXPECT_EQ(color_map[image.lPixels[0]], 0); // First color should map to index 0.
  EXPECT_EQ(color_map[image.lPixels[1]], 1); // Second color should map to index 1.
}


// Test for write_pixel_indices function for Small Pixels (1 byte each): checks that the correct number of indices are written.
TEST(CompressAOSTest, WritePixelIndicesSmall) {
  std::ostringstream output; // Output file.
  std::map<SmallPixel, size_t> color_map; // Associates pixel colors with its corresponding index.
  PPMImageAOS image = createAOSSmallImage(); // Create an instance of PPMImageAOS for testing.
  color_map[image.sPixels[0]] = 0; // First color(pixel) is assigned index 0.
  color_map[image.sPixels[1]] = 1; // Second color(pixel) is assigned index 1.
  color_map[image.sPixels[2]] = 2; // Third color(pixel) is assigned index 2.
  color_map[image.sPixels[3]] = 3; // Fourth color(pixel) is assigned index 3.
  output.str(""); // To remove any previous content from the output stream.
  write_pixel_indices<SmallPixel, uint8_t>(output, image.sPixels, color_map); // Writes the pixel indices.
  EXPECT_EQ(output.str().size(), 4); // Should write 4 bytes (1 byte per pixel) to the output stream since there are 4 colors/pixels in the image.sPixels vector.
}


// Test for write_pixel_indices function for Large Pixels (2 bytes each): checks that the correct number of indices are written.
TEST(CompressAOSTest, WritePixelIndicesLarge) {
  std::ostringstream output; // Output file.
  PPMImageAOS image = createAOSLargeImage(); // Create an instance of PPMImageAOS with Large Pixels for testing.
  std::map<LargePixel, size_t> color_map; // Associates pixel colors with its corresponding index.
  color_map[image.lPixels[0]] = 0; // First color(pixel) is assigned index 0.
  color_map[image.lPixels[1]] = 1; // Second color(pixel) is assigned index 1.
  color_map[image.lPixels[2]] = 2; // Third color(pixel) is assigned index 2.
  color_map[image.lPixels[3]] = 3; // Fourth color(pixel) is assigned index 3.
  output.str(""); // To remove any previous content from the output stream.
  write_pixel_indices<LargePixel, uint16_t>(output, image.lPixels, color_map); // Writes the pixel indices.
  EXPECT_EQ(output.str().size(), 8); // Should write 8 bytes (2 bytes per pixel) to the output stream since there are 4 colors/pixels in the image.lPixels vector.
}


// Test for process_small_pixel_image function: checks the output size matches the expected when processing small pixels.
TEST(CompressAOSTest, ProcessSmallPixelImage) {
  std::ostringstream output; // Output file.
  PPMImageAOS const image = createAOSSmallImage(); // Create an instance of PPMImageAOS for testing.
  process_small_pixel_image(output, image); // Pass the PPMImageAOS image defined above.
  // Check if the actual output (header + color table + indices) is as expected.
  // The header will be C6 (3 bytes including space char) + Width (2 bytes including space char) + Height (2 bytes) + Max.Value (4 bytes) + the color table size (1 byte) + "\n" (1 byte) = 13 bytes.
  // The color table is 4*3 = 12 bytes.
  // The indices are 4 bytes.
  // Total = 13 + 12 + 4 = 29 bytes.
  size_t const expected_size = 13 + 12 + 4; // 13 bytes for the header, 12 bytes for color table + 4 bytes for indices (1 byte for each color/pixel).
  EXPECT_EQ(output.str().size(), expected_size); // Compare actual and expected output.
}


// Test for process_large_pixel_image function: checks the output size matches the expected when processing large pixels.
TEST(CompressAOSTest, ProcessLargePixelImage) {
  std::ostringstream output; // Output file.
  PPMImageAOS const image = createAOSLargeImage(); // Create an instance of PPMImageAOS with Large Pixels for testing.
  output.str(""); // Clear content from output stream.
  process_large_pixel_image(output, image);
  // Check if the actual output (header + color table + indices) is as expected.
  // The header will be C6 (3 bytes including space char) + Width (2 bytes including space char) + Height (2 bytes) + Max.Value (6 bytes) + the color table size (1 byte) + "\n" (1 byte) = 15 bytes.
  // The color table is 4*6 = 24 bytes.
  // The indices are 4 bytes.
  // Total = 15 + 24 + 4 = 43 bytes.
  size_t const expected_size = 15 + 24 + 4; // 15 bytes for the header, 24 bytes for color table + 4 indices (4 colors/pixels).
  EXPECT_EQ(output.str().size(), expected_size); // Compare actual and expected output.
}
// NOLINTEND(cppcoreguidelines-avoid-adjacent-parameters)
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
