#include "gtest/gtest.h"
#include <sstream>
#include "../imgsoa/compresssoa.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTBEGIN(cppcoreguidelines-avoid-adjacent-parameters)

namespace {
  // We create a function to create SOAImage structures for 3 bytes pixel to be able to perform the tests.
  auto createSOAImage3() -> SOAImage {
    SOAImage image;
    image.width           = 2;
    image.height          = 2;
    image.max_color_value = 255;
    // 4 colors: red (255,0,0), green (0,255,0), blue (0,0,255) and white (255,255,255).
    image.red1_components   = {255, 0, 0, 255};  // 4 pixels, then, 4 red component values.
    image.green1_components = {0, 255, 0, 255};  // 4 pixels, then, 4 green component values.
    image.blue1_components  = {0, 0, 255, 255};  // 4 pixels, then, 4 blue component values.
    return image;
  }

  // We create a function to create SOAImage structures for 6 bytes pixel to be able to perform the tests.
  auto createSOAImage6() -> SOAImage {
    SOAImage image;
    image.width           = 2;
    image.height          = 2;
    image.max_color_value = 65535;
    // Again, 4 colors:
    image.red2_components   = {65535, 0, 0, 32768};  // 4 pixels, then, 4 red component values.
    image.green2_components = {32768, 65535, 0,
                               65535};               // 4 pixels, then, 4 green component values.
    image.blue2_components  = {0, 0, 32768, 65535};  // 4 pixels, then, 4 blue component values.
    return image;
  }
}

// Test for writing the header of an Image with 3-byte Pixels.
TEST(CompressSOATests, WriteHeader3) {
  SOAImage const image = createSOAImage3();
  std::ostringstream output;
  write_header(output, image, 4); // 4 unique colors.
  std::string const expected_header = "C6 2 2 255 4\n";
  EXPECT_EQ(output.str(), expected_header);
}


// Test for writing the header of an Image with 6-byte Pixels.
TEST(CompressSOATests, WriteHeader6) {
  SOAImage const image = createSOAImage6();
  std::ostringstream output;
  write_header(output, image, 4); // 4 unique colors.
  std::string const expected_header = "C6 2 2 65535 4\n";
  EXPECT_EQ(output.str(), expected_header);
}


// Test for generate_color_table function with 3-byte Pixels
TEST(CompressSOATests, GenerateColorTable3) {
  SOAImage const image = createSOAImage3();
  AuxPixelVects<uint8_t> unique_colors; // holds unique colors.
  auto color_map = generate_color_table(image.red1_components,image.green1_components,image.blue1_components,unique_colors);
  EXPECT_EQ(unique_colors.red.size(), 4); // Expecting 4 unique colors.
  EXPECT_EQ(color_map.size(), 4); // Check if the color map has 4 entries.
}

// Test for generate_color_table function with 6-byte Pixels
TEST(CompressSOATests, GenerateColorTable6) {
  SOAImage const image = createSOAImage6();
  AuxPixelVects<uint16_t> unique_colors; // holds unique colors.
  auto color_map = generate_color_table(image.red2_components,image.green2_components,image.blue2_components,unique_colors);
  EXPECT_EQ(unique_colors.red.size(), 4); // Expecting 4 unique colors.
  EXPECT_EQ(color_map.size(), 4); // Check if the color map has 4 entries.
}


// Test process_small_pixel_image (for 3-byte pixel image)
TEST(CompressSOATests, ProcessSmallPixelImage) {
  SOAImage const Image = createSOAImage3();
  std::ostringstream output;
  EXPECT_NO_THROW(process_small_pixel_image(output, Image)); // Check that everything works correctly without exceptions.
  // Expected output (as vector of bytes):
  std::vector<uint8_t> const expected_header = std::vector<uint8_t>{'C', '6', ' ', '2', ' ', '2', ' ', '2', '5', '5', ' ', '4', '\n'};
  std::vector<uint8_t> const expected_pixels = {
    0xFF, 0x00, 0x00, // Color 1: (255, 0, 0)
    0x00, 0xFF, 0x00, // Color 2: (0, 255, 0)
    0x00, 0x00, 0xFF, // Color 3: (0, 0, 255)
    0xFF, 0xFF, 0xFF,  // Color 4: (255, 255, 255)
    0x00, // Index for (255, 0, 0) -> Color 1
    0x01, // Index for (0, 255, 0) -> Color 2
    0x02, // Index for (0, 0, 255) -> Color 3
    0x03  // Index for (255, 255, 255) -> Color 4
  }; // In total, 29 bytes.
  size_t const expected_output_size = 29;
  EXPECT_EQ(output.str().size(), expected_output_size);
}


// Test process_large_pixel_image (for 6-byte pixel image)
TEST(CompressSOATests, ProcessLargePixelImage) {
  SOAImage const Image = createSOAImage6();
  std::ostringstream output;
  EXPECT_NO_THROW(process_large_pixel_image(output, Image)); // Check that everything works correctly without exceptions.
  // In total, 43 bytes.
  size_t const expected_output_size = 43; // For the header, color table and indices.
  EXPECT_EQ(output.str().size(), expected_output_size);
}

// NOLINTEND(cppcoreguidelines-avoid-adjacent-parameters)
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
