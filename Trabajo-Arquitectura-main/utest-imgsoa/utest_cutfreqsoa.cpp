#include "gtest/gtest.h"
#include "../imgsoa/cutfreqsoa.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace {
  // Function to initialize a 3-byte image for testing:
  void Image3SOA(SOAImage & image, std::vector<uint8_t>& reds,
                 std::vector<uint8_t>& greens, std::vector<uint8_t>& blues) {
    image.red1_components   = reds;
    image.green1_components = greens;
    image.blue1_components  = blues;
    image.max_color_value   = 255;
  }

  // Function to initialize a 6-byte image for testing:
  void Image6SOA(SOAImage & image, std::vector<uint16_t>& reds,
                 std::vector<uint16_t>& greens, std::vector<uint16_t>& blues) {
    image.red2_components   = reds;
    image.green2_components = greens;
    image.blue2_components  = blues;
    image.max_color_value   = 65535;
  }
}

// sortColors function test
TEST(CutFreqSoaTest, SortColors) {
  std::unordered_map<uint32_t, size_t> const color_freq = {
    {0xFF0000, 3}, // Red
    {0x00FF00, 2}, // Green
    {0x0000FF, 2}, // Blue
    {0xFFFF00, 1}  // Yellow
  };
  std::vector<uint32_t> colors = {16776960, 16711680, 65280, 255}; // This decimal values corresponds to the hexadecimal values of the color pixels.
  sortColors<uint8_t,uint32_t>(colors, color_freq); // Call the function to get actual output.
  ASSERT_EQ(colors[0], 0xFFFF00); // Yellow (least frequent)
  ASSERT_EQ(colors[1], 0x0000FF); // Blue (2 frequency, sorted after Yellow)
  ASSERT_EQ(colors[2], 0x00FF00); // Green (2 frequency, sorted after Green)
  ASSERT_EQ(colors[3], 0xFF0000); // Red (most frequent)
}


// Test FindNearestColors function
TEST(FindNearestColorsTest, FindNearestColor) {
  std::unordered_set<uint32_t> const to_remove = {0xFF0000}; // Red
  std::unordered_set<uint32_t> const to_keep = {0x00FF00, 0x0000FF}; // Green and Blue
  std::unordered_map<uint32_t, uint32_t> replacement_map;
  FindNearestColors<uint8_t, uint32_t>(to_remove,replacement_map, to_keep);
  ASSERT_EQ(replacement_map[0xFF0000], 255); // Closest color to red should be blue (based on Euclidean distance)
}


// Test removeColors function with n = 2 and 3-byte pixels
TEST(RemoveColorsTest, Remove2Colors3Byte) {
  SOAImage image;
  Image3SOA(image, {255, 0, 0, 255}, {0, 255, 0, 0}, {0, 0, 255, 0});
  // Green and Blue should be replaced since they are the least frequent colors.
  // Should be replaced by Red.
  removeColors<uint8_t, uint32_t>(2, image.red1_components, image.green1_components, image.blue1_components);
  // Check green pixel was replaced to the closest red:
  ASSERT_EQ(image.red1_components[1], 255);
  ASSERT_EQ(image.green1_components[1], 0);
  ASSERT_EQ(image.blue1_components[1], 0);
  // Check blue pixel was replaced to the closest red:
  ASSERT_EQ(image.red1_components[2], 255);
  ASSERT_EQ(image.green1_components[2], 0);
  ASSERT_EQ(image.blue1_components[2], 0);

  // Check the red colors remain unchanged:
  ASSERT_EQ(image.red1_components[0], 255);
  ASSERT_EQ(image.green1_components[0], 0);
  ASSERT_EQ(image.blue1_components[0], 0);

  ASSERT_EQ(image.red1_components[3], 255);
  ASSERT_EQ(image.green1_components[3], 0);
  ASSERT_EQ(image.blue1_components[3], 0);

}


// Test removeColors function with n = 1 and 3-byte pixels.
TEST(RemoveColorsTest, Remove1Color3Byte) {
  SOAImage image;
  Image3SOA(image, {255, 0, 0, 255}, {0, 255, 0, 0}, {0, 0, 255, 0});
  //Blue should be replaced by closest color
  removeColors<uint8_t, uint32_t>(1, image.red1_components, image.green1_components, image.blue1_components);
  // Check blue pixel was replaced to the closest red:
  ASSERT_EQ(image.red1_components[2], 255);
  ASSERT_EQ(image.green1_components[2], 0);
  ASSERT_EQ(image.blue1_components[2], 0);

  // Check the other colors remain unchanged:
  // First Red Pixel remains unchanged.
  ASSERT_EQ(image.red1_components[0], 255);
  ASSERT_EQ(image.green1_components[0], 0);
  ASSERT_EQ(image.blue1_components[0], 0);
  // Green Pixel remains unchanged.
  ASSERT_EQ(image.red1_components[1], 0);
  ASSERT_EQ(image.green1_components[1], 255);
  ASSERT_EQ(image.blue1_components[1], 0);
  // Second Red Pixel remains unchanged.
  ASSERT_EQ(image.red1_components[3], 255);
  ASSERT_EQ(image.green1_components[3], 0);
  ASSERT_EQ(image.blue1_components[3], 0);
}


// Test removeColors function with n = 2 and 6-byte pixels
TEST(RemoveColorsTest, Remove2Colors6Byte) {
  SOAImage image;
  Image6SOA(image, {65535, 0, 0, 65535}, {0, 65535, 0, 0}, {0, 0, 65535, 0});
  // Green and Blue should be replaced since they are the least frequent colors.
  // Should be replaced by Red.
  removeColors<uint16_t, uint64_t>(2, image.red2_components, image.green2_components, image.blue2_components);
  // Check green pixel was replaced to the closest red:
  ASSERT_EQ(image.red2_components[1], 65535);
  ASSERT_EQ(image.green2_components[1], 0);
  ASSERT_EQ(image.blue2_components[1], 0);
  // Check blue pixel was replaced to the closest red:
  ASSERT_EQ(image.red2_components[2], 65535);
  ASSERT_EQ(image.green2_components[2], 0);
  ASSERT_EQ(image.blue2_components[2], 0);

  // Check the red colors remain unchanged:
  ASSERT_EQ(image.red2_components[0], 65535);
  ASSERT_EQ(image.green2_components[0], 0);
  ASSERT_EQ(image.blue2_components[0], 0);
  ASSERT_EQ(image.red2_components[3], 65535);
  ASSERT_EQ(image.green2_components[3], 0);
  ASSERT_EQ(image.blue2_components[3], 0);
}


// Test removeColors function with n = 1 and 6-byte pixels.
TEST(RemoveColorsTest, Remove1Color6Byte) {
  SOAImage image;
  Image6SOA(image, {65535, 0, 0, 65535}, {0, 65535, 0, 0}, {0, 0, 65535, 0});
  //Blue should be replaced by closest color
  removeColors<uint16_t, uint64_t>(1, image.red2_components, image.green2_components, image.blue2_components);
  // Check blue pixel was replaced to the closest red:
  ASSERT_EQ(image.red2_components[2], 65535);
  ASSERT_EQ(image.green2_components[2], 0);
  ASSERT_EQ(image.blue2_components[2], 0);

  // Check the other colors remain unchanged:
  // First Red Pixel remains the same.
  ASSERT_EQ(image.red2_components[0], 65535);
  ASSERT_EQ(image.green2_components[0], 0);
  ASSERT_EQ(image.blue2_components[0], 0);
  // Green Pixel remains the same.
  ASSERT_EQ(image.red2_components[1], 0);
  ASSERT_EQ(image.green2_components[1], 65535);
  ASSERT_EQ(image.blue2_components[1], 0);
  // Second Red Pixel remains the same.
  ASSERT_EQ(image.red2_components[3], 65535);
  ASSERT_EQ(image.green2_components[3], 0);
  ASSERT_EQ(image.blue2_components[3], 0);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
