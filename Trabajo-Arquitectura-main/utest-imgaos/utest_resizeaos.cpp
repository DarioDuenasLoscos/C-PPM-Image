#include "gtest/gtest.h"
#include "../imgaos/resizeaos.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

// Test interpolatePixel function
TEST(ResizeTestsAOS, InterpolatePixel) {
  InterpolationData<SmallPixel> data = {.c00={},.c10={},.c01={},.c11={},.x_weight=0.5,.y_weight=0.5,.max_color_value=255};

  // Define the corner pixels
  data.c00 = {.red=255, .green=0, .blue=0};  // Red
  data.c10 = {.red=0, .green=255, .blue=0};  // Green
  data.c01 = {.red=0, .green=0, .blue=255};  // Blue
  data.c11 = {.red=255, .green=255, .blue=0}; // Yellow

  SmallPixel const result = interpolatePixel(data);

  // Expected color should be a blend of the four colors
  EXPECT_EQ(result.red, 127);   // 255 * (1−0.5) + 0 * 0.5 = 127.5 --> 127
  EXPECT_EQ(result.green, 127); // 127.5 * (1−0.5) + 127.5 * 0.5 = 127.5 --> 127
  EXPECT_EQ(result.blue, 63);  // 0 * (1 - 0.5) + 127.5 * 0.5 = 63.75 --> 63
}


// Test calculateCoords function
TEST(ResizeTestsAOS, CalculateCoords) {
  CoordinateParams const params = {.x_prime=1, .y_prime=1, .inputSize={.width=4,.height=4}, .outputSize={.width=2,.height=2}};
  InterpolationCoords const coords = calculateCoords(params);
  EXPECT_EQ(coords.x_l, 3); // x = x_prime * x_ratio = 1 * 3.0 = 3.0
  EXPECT_EQ(coords.x_h, 3);
  EXPECT_EQ(coords.y_l, 3);
  EXPECT_EQ(coords.y_h, 3);
  EXPECT_NEAR(coords.x_weight, 0.0, 1e-6);
  EXPECT_NEAR(coords.y_weight, 0.0, 1e-6);
}


// Test resizeImageAOS from 2x2 to 4x4 for images with Small Pixels.
TEST(ResizeTestsAOS, ResizeSmallToLargeSmallPixel) {
    PPMImageAOS image;
    image.width = 2;
    image.height = 2;
    image.max_color_value = 255;
    image.sPixels = {
        {.red=255, .green=0, .blue=0},   // Pixel 1
        {.red=0, .green=255, .blue=0},   // Pixel 2
        {.red=0, .green=0, .blue=255},   // Pixel 3
        {.red=255, .green=255, .blue=0}  // Pixel 4
    };
    // Call resize function to increase size of the image from 2x2 to 4x4
    resizeImageAOS(4, image, 4); // Actual Output.
    // Check the size of the output image after calling the resize function.
    EXPECT_EQ(image.width, 4);
    EXPECT_EQ(image.height, 4);
    // Check pixel values of the output image (resized):
    // x_ratio = y_ratio = 1/3
    EXPECT_EQ(image.sPixels[0].red, 255);   // Pixel (0,0)
    EXPECT_EQ(image.sPixels[0].green, 0);
    EXPECT_EQ(image.sPixels[0].blue, 0);

    EXPECT_EQ(image.sPixels[1].red, 170);   // Pixel (1,0)
    EXPECT_EQ(image.sPixels[1].green, 85);
    EXPECT_EQ(image.sPixels[1].blue, 0);

    EXPECT_EQ(image.sPixels[2].red, 85);   // Pixel (2,0)
    EXPECT_EQ(image.sPixels[2].green, 170);
    EXPECT_EQ(image.sPixels[2].blue, 0);

    EXPECT_EQ(image.sPixels[3].red, 0);   // Pixel (3,0)
    EXPECT_EQ(image.sPixels[3].green, 255);
    EXPECT_EQ(image.sPixels[3].blue, 0);
}


// Test resizeImageAOS from 2x2 to 4x4 for images with Large Pixels.
TEST(ResizeTestsAOS, ResizeSmallToLargeLargePixel) {
    PPMImageAOS image;
    image.width = 2;
    image.height = 2;
    image.max_color_value = 65535; // 16-bit color depth.
    image.lPixels = {
        {.red=65535, .green=0, .blue=0},   // Pixel 1
        {.red=0, .green=65535, .blue=0},   // Pixel 2
        {.red=0, .green=0, .blue=65535},   // Pixel 3
        {.red=65535, .green=65535, .blue=0} // Pixel 4
    };
    // Call resize function to increase size of the image from 2x2 to 4x4
    resizeImageAOS(4, image, 4); // Actual output.
    // Check the size of the output image
    EXPECT_EQ(image.width, 4);
    EXPECT_EQ(image.height, 4);

  // Check pixel values of the output image (resized):
  // x_ratio = y_ratio = 2/3
  EXPECT_EQ(image.lPixels[0].red, 65535);   // Pixel (0,0)
  EXPECT_EQ(image.lPixels[0].green, 0);
  EXPECT_EQ(image.lPixels[0].blue, 0);

  EXPECT_EQ(image.lPixels[1].red, 43690);   // Pixel (1,0)
  EXPECT_EQ(image.lPixels[1].green, 21845);
  EXPECT_EQ(image.lPixels[1].blue, 0);

  EXPECT_EQ(image.lPixels[2].red, 21845);   // Pixel (2,0)
  EXPECT_EQ(image.lPixels[2].green, 43690);
  EXPECT_EQ(image.lPixels[2].blue, 0);

  EXPECT_EQ(image.lPixels[3].red, 0);   // Pixel (3,0)
  EXPECT_EQ(image.lPixels[3].green, 65535);
  EXPECT_EQ(image.lPixels[3].blue, 0);
}


// Test resizeImageAOS from 4x4 to 2x2 for images with Small Pixels.
TEST(ResizeTestsAOS, ResizeLargeToSmallSmallPixel) {
  PPMImageAOS image;
  image.width = 4;
  image.height = 4;
  image.max_color_value = 255;
  image.sPixels = {
    {.red=255, .green=0, .blue=0},   // Pixel 1
    {.red=0, .green=255, .blue=0},   // Pixel 2
    {.red=0, .green=0, .blue=255},   // Pixel 3
    {.red=255, .green=255, .blue=0}, // Pixel 4
    {.red=0, .green=255, .blue=255}, // Pixel 5
    {.red=255, .green=0, .blue=255}, // Pixel 6
    {.red=255, .green=255, .blue=255}, // Pixel 7
    {.red=0, .green=0, .blue=0},     // Pixel 8
    {.red=128, .green=128, .blue=128}, // Pixel 9
    {.red=192, .green=192, .blue=192}, // Pixel 10
    {.red=255, .green=128, .blue=64}, // Pixel 11
    {.red=128, .green=64, .blue=255}, // Pixel 12
    {.red=64, .green=255, .blue=128}, // Pixel 13
    {.red=0, .green=128, .blue=255},  // Pixel 14
    {.red=0, .green=255, .blue=128},  // Pixel 15
    {.red=128, .green=0, .blue=255}   // Pixel 16
  };
  // Call resize function to reduce size of the image from 4x4 to 2x2.
  resizeImageAOS(2, image, 2); // Actual Output.
  // Compare actual with expected results.
  EXPECT_EQ(image.width, 2);
  EXPECT_EQ(image.height, 2);

  EXPECT_EQ(image.sPixels[1].red, 255);  // Expected interpolated values.
  EXPECT_EQ(image.sPixels[1].green, 255);
  EXPECT_EQ(image.sPixels[1].blue, 0);
}


// Test resizeImageAOS from 4x4 to 2x2 for Large Pixel image.
TEST(ResizeTestsAOS, ResizeLargeToSmallLargePixel) {
    PPMImageAOS image;
    image.width = 4;
    image.height = 4;
    image.max_color_value = 65535;  // Max value for 16-bit color components
    image.lPixels = {
        {.red=65535, .green=0, .blue=0},
        {.red=0, .green=65535, .blue=0},
        {.red=0, .green=0, .blue=65535},
        {.red=65535, .green=65535, .blue=0},

        {.red=0, .green=65535, .blue=65535},
        {.red=65535, .green=0, .blue=65535},
        {.red=65535, .green=65535, .blue=65535},
        {.red=0, .green=0, .blue=0},

        {.red=32768, .green=32768, .blue=32768},
        {.red=49152, .green=49152, .blue=49152},
        {.red=57344, .green=57344, .blue=57344},
        {.red=65535, .green=65535, .blue=65535},

        {.red=255, .green=255, .blue=255},
        {.red=128, .green=128, .blue=128},
        {.red=0, .green=255, .blue=255},
        {.red=65535, .green=0, .blue=0}
    };

  // Call resize function to reduce size of the image from 4x4 to 2x2
    resizeImageAOS(2, image, 2); // Actual Output.
    // Check the resized image parameters
    EXPECT_EQ(image.width, 2);
    EXPECT_EQ(image.height, 2);
    EXPECT_EQ(image.max_color_value, 65535);

    // Check color values at the resized pixels. Determined by interpolation.
    // Top-left pixel:
    EXPECT_EQ(image.lPixels[0].red, 65535);
    EXPECT_EQ(image.lPixels[0].green, 0);
    EXPECT_EQ(image.lPixels[0].blue, 0);

    // Top-right pixel:
    EXPECT_EQ(image.lPixels[1].red, 65535);
    EXPECT_EQ(image.lPixels[1].green, 65535);
    EXPECT_EQ(image.lPixels[1].blue, 0);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
