#include "gtest/gtest.h"
#include "../imgaos/cutfreqaos.hpp"
#include "../common/binaryio.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace {
  // We create a function to create PPMImageAOS structures for 3 bytes pixel to be able to perform the tests.
  void createAOSSmallImg(PPMImageAOS & image, std::vector<SmallPixel>& smallPix) {
    image.sPixels         = smallPix;
    image.max_color_value = 255;
  }

  // We create a function to create PPMImageAOS structures for 6 bytes pixel to be able to perform the tests.
  void createAOSLargeImg(PPMImageAOS & image, std::vector<LargePixel>& largePix) {
    image.lPixels         = largePix;
    image.max_color_value = 65535;
  }
}
// Test FindNearestColors function for images with small pixels.
TEST(CutFreqAOSTest, TestFindNearestColorsSmall) {
  std::unordered_set<SmallPixel> const to_remove = {{.red=255, .green=0, .blue=0}, {.red=0, .green=0, .blue=255}}; // Remove Red and Blue colors.
  std::unordered_set<SmallPixel> const to_keep = {{.red=0, .green=255, .blue=0}, {.red=255, .green=255, .blue=0}, {.red=255, .green=255, .blue=255}}; // Keep Green, Cyan and White colors.
  std::unordered_map<SmallPixel, SmallPixel> replacement_map;
  FindNearestColors(to_remove, replacement_map, to_keep);
  // Red (255,0,0) color should be replaced by cyan (255,255,0)
  SmallPixel const red = {.red=255,.green=0,.blue=0};
  SmallPixel const cyan = {.red=255, .green=255, .blue=0};
  EXPECT_EQ(replacement_map[red], cyan);
}

// Test FindNearestColors function for images with large pixels.
TEST(CutFreqAOSTest, TestFindNearestColorsLarge) {
  std::unordered_set<LargePixel> const to_remove = {{.red=65535, .green=0, .blue=0}, {.red=0, .green=0, .blue=65535}}; // Remove Red and Blue colors.
  std::unordered_set<LargePixel> const to_keep = {{.red=0, .green=65535, .blue=0}, {.red=65535, .green=65535, .blue=0}, {.red=65535, .green=65535, .blue=65535}}; // Keep Green, Cyan and White colors.
  std::unordered_map<LargePixel, LargePixel> replacement_map;
  FindNearestColors(to_remove, replacement_map, to_keep);
  // Red (65535,0,0) color should be replaced by cyan (65535,65535,0)
  LargePixel const red = {.red=65535, .green=0, .blue=0};
  LargePixel const cyan = {.red=65535, .green=65535, .blue=0};
  EXPECT_EQ(replacement_map[red], cyan);
}


// Test removeColors for n = 1 and images of Small Pixels.
TEST(RemoveColorsTest, Remove1ColorSmall) {
  std::vector<SmallPixel> pixels = {
    {.red=255, .green=0, .blue=0},  // Red
    {.red=0, .green=255, .blue=0},  // Green
    {.red=0, .green=0, .blue=255},  // Blue
  {.red=255, .green=0, .blue=0},  // Red
  };
  PPMImageAOS image;
  createAOSSmallImg(image, pixels);
  // Remove the least frequent color (just 1 color should be removed)
  removeColors<SmallPixel>(1, image.sPixels);
  // Verify that the number of pixels has not changed (color replaced, not removed)
  ASSERT_EQ(image.sPixels.size(), 4);
  // Check that the least frequent color Blue is replaced with Red
  EXPECT_EQ(image.sPixels[2].red, 255);   // Red component should be 255 (Red replacement)
  EXPECT_EQ(image.sPixels[2].green, 0);   // Green component should be 0
  EXPECT_EQ(image.sPixels[2].blue, 0);    // Blue component should be 0
  // Check the other colors remain unchanged:
  EXPECT_EQ(image.sPixels[0].red, 255);
  EXPECT_EQ(image.sPixels[0].green, 0);
  EXPECT_EQ(image.sPixels[0].blue, 0);

  EXPECT_EQ(image.sPixels[1].red, 0);
  EXPECT_EQ(image.sPixels[1].green, 255);
  EXPECT_EQ(image.sPixels[1].blue, 0);

  EXPECT_EQ(image.sPixels[3].red, 255);
  EXPECT_EQ(image.sPixels[3].green, 0);
  EXPECT_EQ(image.sPixels[3].blue, 0);
}


// Test removeColors for n = 2 and images of Small Pixels.
TEST(RemoveColorsTest, Remove2ColorSmall) {
  std::vector<SmallPixel> pixels = {
    {.red=255, .green=0, .blue=0},  // Red
    {.red=0, .green=255, .blue=0},  // Green
    {.red=0, .green=0, .blue=255},  // Blue
  {.red=255, .green=0, .blue=0},  // Red
  };
  PPMImageAOS image;
  createAOSSmallImg(image, pixels);
  // Remove the least frequent colors (2 color should be removed)
  removeColors<SmallPixel>(2, image.sPixels);
  // Verify that the number of pixels has not changed (color replaced, not removed)
  ASSERT_EQ(image.sPixels.size(), 4);
  // Check that the least frequent colors Blue and Green are replaced with Red
  EXPECT_EQ(image.sPixels[2].red, 255);   // Red component should be 255 (Red replacement)
  EXPECT_EQ(image.sPixels[2].green, 0);   // Green component should be 0
  EXPECT_EQ(image.sPixels[2].blue, 0);    // Blue component should be 0

  EXPECT_EQ(image.sPixels[1].red, 255);
  EXPECT_EQ(image.sPixels[1].green, 0);
  EXPECT_EQ(image.sPixels[1].blue, 0);

  // Check the other colors remain unchanged:
  EXPECT_EQ(image.sPixels[0].red, 255);
  EXPECT_EQ(image.sPixels[0].green, 0);
  EXPECT_EQ(image.sPixels[0].blue, 0);

  EXPECT_EQ(image.sPixels[3].red, 255);
  EXPECT_EQ(image.sPixels[3].green, 0);
  EXPECT_EQ(image.sPixels[3].blue, 0);
}


// Test removeColors for n = 1 and images of Large Pixels.
TEST(RemoveColorsTest, Remove1ColorLarge) {
  std::vector<LargePixel> pixels = {
    {.red=65535, .green=0, .blue=0},  // Red
    {.red=0, .green=65535, .blue=0},  // Green
    {.red=0, .green=0, .blue=65535},  // Blue
  {.red=0, .green=65535, .blue=0},  // Red
  };
  PPMImageAOS image;
  createAOSLargeImg(image, pixels);
  // Remove the least frequent colors (1 color should be removed)
  removeColors<LargePixel>(1, image.lPixels);
  // Verify that the number of pixels has not changed (color replaced, not removed)
  ASSERT_EQ(image.lPixels.size(), 4);
  // Check that the least frequent color is Blue and is replaced with Green
  EXPECT_EQ(image.lPixels[2].red, 0);   // Red component should be 0
  EXPECT_EQ(image.lPixels[2].green, 65535);   // Green component should be 65535
  EXPECT_EQ(image.lPixels[2].blue, 0);    // Blue component should be 0

  // Check the other colors remain unchanged:
  EXPECT_EQ(image.lPixels[0].red, 65535);
  EXPECT_EQ(image.lPixels[0].green, 0);
  EXPECT_EQ(image.lPixels[0].blue, 0);

  EXPECT_EQ(image.lPixels[1].red, 0);
  EXPECT_EQ(image.lPixels[1].green, 65535);
  EXPECT_EQ(image.lPixels[1].blue, 0);

  EXPECT_EQ(image.lPixels[3].red, 0);
  EXPECT_EQ(image.lPixels[3].green, 65535);
  EXPECT_EQ(image.lPixels[3].blue, 0);
}


// Test removeColors for n = 2 and images of Large Pixels.
TEST(RemoveColorsTest, Remove2ColorLarge) {
  std::vector<LargePixel> pixels = {
    {.red=65535, .green=0, .blue=0},  // Red
    {.red=0, .green=65535, .blue=0},  // Green
    {.red=0, .green=0, .blue=65535},  // Blue
  {.red=0, .green=65535, .blue=0},  // Red
  };
  PPMImageAOS image;
  createAOSLargeImg(image, pixels);
  // Remove the least frequent colors (2 color should be removed)
  removeColors<LargePixel>(2, image.lPixels);
  // Verify that the number of pixels has not changed (color replaced, not removed)
  ASSERT_EQ(image.lPixels.size(), 4);
  // Check that the least frequent colors are Blue and Red which are replaced with Green
  EXPECT_EQ(image.lPixels[2].red, 0);   // Red component should be 0
  EXPECT_EQ(image.lPixels[2].green, 65535);   // Green component should be 65535
  EXPECT_EQ(image.lPixels[2].blue, 0);    // Blue component should be 0

  EXPECT_EQ(image.lPixels[0].red, 0);
  EXPECT_EQ(image.lPixels[0].green, 65535);
  EXPECT_EQ(image.lPixels[0].blue, 0);

  // Check the other colors remain unchanged:
  EXPECT_EQ(image.lPixels[1].red, 0);
  EXPECT_EQ(image.lPixels[1].green, 65535);
  EXPECT_EQ(image.lPixels[1].blue, 0);

  EXPECT_EQ(image.lPixels[3].red, 0);
  EXPECT_EQ(image.lPixels[3].green, 65535);
  EXPECT_EQ(image.lPixels[3].blue, 0);
}


// Test removeColors when frequency is the same for all colors and images of Small Pixels.
TEST(RemoveColorsTest, RemoveColorSmall) {
  std::vector<SmallPixel> pixels = {
    {.red=255, .green=0, .blue=0},  // Red
    {.red=0, .green=255, .blue=0},  // Green
    {.red=0, .green=0, .blue=255},  // Blue
  };
  PPMImageAOS image;
  createAOSSmallImg(image, pixels);
  // Remove the least frequent color
  removeColors<SmallPixel>(1, image.sPixels);
  // Verify that the number of pixels has not changed (color replaced, not removed)
  ASSERT_EQ(image.sPixels.size(), 3);
  // Check that the least frequent color Blue is replaced with Red color because of its Euclidean distance.
  EXPECT_EQ(image.sPixels[2].red, 255);   // Red component should be 255
  EXPECT_EQ(image.sPixels[2].green, 0);   // Green component should be 0
  EXPECT_EQ(image.sPixels[2].blue, 0);    // Blue component should be 0
  // Check the other colors remain unchanged:
  EXPECT_EQ(image.sPixels[0].red, 255);
  EXPECT_EQ(image.sPixels[0].green, 0);
  EXPECT_EQ(image.sPixels[0].blue, 0);

  EXPECT_EQ(image.sPixels[1].red, 0);
  EXPECT_EQ(image.sPixels[1].green, 255);
  EXPECT_EQ(image.sPixels[1].blue, 0);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
