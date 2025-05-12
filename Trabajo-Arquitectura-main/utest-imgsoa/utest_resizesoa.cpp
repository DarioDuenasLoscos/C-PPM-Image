#include "gtest/gtest.h"
#include "../imgsoa/resizesoa.hpp"

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

namespace {
  // Function to initialize a 3-byte image for testing:
  void ImageSOA3(SOAImage & image, std::vector<uint8_t>& reds,
                 std::vector<uint8_t>& greens, std::vector<uint8_t>& blues) {
    image.red1_components   = reds;
    image.green1_components = greens;
    image.blue1_components  = blues;
    image.max_color_value   = 255;
  }

  // Function to initialize a 6-byte image for testing:
  void ImageSOA6(SOAImage & image, std::vector<uint16_t>& reds,
                 std::vector<uint16_t>& greens, std::vector<uint16_t>& blues) {
    image.red2_components   = reds;
    image.green2_components = greens;
    image.blue2_components  = blues;
    image.max_color_value   = 65535;
  }
}

// Test fetchData function for 3-byte images.
TEST(ResizeSOATests, FetchData3) {
  // We define only one pixel:
  std::vector<uint8_t> red = {100};
  std::vector<uint8_t> green = {150};
  std::vector<uint8_t> blue = {200};
  SOAImage image;
  image.width = 1;
  image.height = 1;
  ImageSOA3(image,red, green, blue); // We define the 3-byte SOA Image for the test.
  AuxPixelVectsref<uint8_t> const components{.red=image.red1_components, .green=image.green1_components, .blue=image.blue1_components};
  InterpolationCoords const coords{.x_l=0, .x_h=0, .y_l=0, .y_h=0, .x_weight=0.5, .y_weight=0.5}; // Interpolation coordinates.
  // We call the function we want to test:
  SOAPixelData<uint8_t> const data = fetchData(image, coords, components); // Actual Output
  EXPECT_EQ(data.r00, 100);
  EXPECT_EQ(data.g00, 150);
  EXPECT_EQ(data.b00, 200);
}


// Test fetchData function for 6-byte images.
TEST(ResizeSOATests, FetchData6) {
  // We define only one pixel:
  std::vector<uint16_t> red = {40000};
  std::vector<uint16_t> green = {50000};
  std::vector<uint16_t> blue = {60000};
  SOAImage image;
  image.width = 1;
  image.height = 1;
  ImageSOA6(image,red, green, blue); // We define the 6-byte SOA Image for the test.
  AuxPixelVectsref<uint16_t> const components{.red=image.red2_components, .green=image.green2_components, .blue=image.blue2_components};
  InterpolationCoords const coords{.x_l=0, .x_h=0, .y_l=0, .y_h=0, .x_weight=0.5, .y_weight=0.5}; // Interpolation coordinates.
  // We call the function we want to test:
  SOAPixelData<uint16_t> const data = fetchData(image, coords, components); // Actual Output
  EXPECT_EQ(data.r00, 40000);
  EXPECT_EQ(data.g00, 50000);
  EXPECT_EQ(data.b00, 60000);
}


// Test interpolate function for 3-byte pixels
TEST(ResizeSOATests, Interpolate8Bit) {
  SOAPixelData<uint8_t> const data{
    .r00=100, .r10=150, .r01=200, .r11=250,  // Red components at neighboring pixels
    .g00=50, .g10=100, .g01=150, .g11=200,   // Green components
    .b00=0, .b10=50, .b01=100, .b11=150      // Blue components
};
  InterpolationCoords const coords{.x_l=0, .x_h=1, .y_l=0, .y_h=1, .x_weight=0.6, .y_weight=0.4}; // x_weight = 0.6, y_weight = 0.4
  // Define output vectors to store interpolated color values of a pixel:
  std::vector<uint8_t> const red_output(1);
  std::vector<uint8_t> const green_output(1);
  std::vector<uint8_t> const blue_output(1);
  AuxPixelVectsref<uint8_t> outputComponents {.red=red_output, .green=green_output, .blue=blue_output};

  InterpolationParams const params{.index=0, .max_color_value=255}; // index = 0, max_color_value = 255
  interpolate<uint8_t>(data, coords, outputComponents, params); // Call the function we want to test.

  // Expected interpolated values: 170, 120 and 70.
  EXPECT_EQ(outputComponents.red[0], 170);   // Expected interpolated red value
  EXPECT_EQ(outputComponents.green[0], 120);  // Expected interpolated green value
  EXPECT_EQ(outputComponents.blue[0], 70);   // Expected interpolated blue value
}


// Test interpolate function for 6-byte pixels
TEST(ResizeSOATests, Interpolate16Bit) {
  SOAPixelData<uint16_t> const data{
    .r00=30000, .r10=35000, .r01=40000, .r11=45000,  // Red components
    .g00=10000, .g10=15000, .g01=20000, .g11=25000,  // Green components
    .b00=5000, .b10=10000, .b01=15000, .b11=20000    // Blue components
};

  InterpolationCoords const coords{.x_l=0, .x_h=1, .y_l=0, .y_h=1, .x_weight=0.25, .y_weight=0.75}; // x_weight = 0.25, y_weight = 0.75
  // Define output vectors to store interpolated color values of a pixel:
  std::vector<uint16_t> const red_output(1);
  std::vector<uint16_t> const green_output(1);
  std::vector<uint16_t> const blue_output(1);
  AuxPixelVectsref<uint16_t> outputComponents{.red=red_output, .green=green_output, .blue=blue_output};

  InterpolationParams const params{.index=0, .max_color_value=65535}; // index = 0, max_color_value = 65535

  // Run the interpolation
  interpolate<uint16_t>(data, coords, outputComponents, params); // Call the function we want to test.

  // Expected interpolated values
  EXPECT_EQ(outputComponents.red[0], 38750);   // Expected interpolated red value
  EXPECT_EQ(outputComponents.green[0], 18750); // Expected interpolated green value
  EXPECT_EQ(outputComponents.blue[0], 13750);  // Expected interpolated blue value
}



// Test for calculateCoords function
TEST(ResizeSOATests, CalcCoors) {
  SOAImage inputImage;
  inputImage.width = 10;
  inputImage.height = 10;
  ResizeParams const params = {.x_prime=5,.y_prime=5,.newWidth=20,.newHeight=20}; // Resize Parameters.
  InterpolationCoords const coords = calculateCoords(params, inputImage); // Call function we want to test.

  EXPECT_EQ(coords.x_l, 2);         // Lower bound for x
  EXPECT_EQ(coords.x_h, 3);         // Upper bound for x
  EXPECT_NEAR(coords.x_weight, 0.3684, 1e-4); // x_weight should be 0.3684

  EXPECT_EQ(coords.y_l, 2);         // Lower bound for y
  EXPECT_EQ(coords.y_h, 3);         // Upper bound for y
  EXPECT_NEAR(coords.y_weight, 0.3684, 1e-4); // y_weight should be 0.3684
}


// Test resizeImageSOA from 4x4 to 2x2 for 3-byte pixel images
TEST(ResizeSOATests, ResizeLargeToSmall3) {
  // We define the 8-bit color component values for a 4x4 image.
  std::vector<uint8_t> red = {255, 0, 0, 255, 0, 255, 0, 0, 255, 0, 0, 255, 255, 0, 255, 0};
  std::vector<uint8_t> green = {0, 255, 0, 255, 255, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255};
  std::vector<uint8_t> blue = {0, 0, 255, 0, 255, 0, 255, 0, 0, 0, 255, 255, 0, 255, 0, 255};
  SOAImage inputImage; // Initialize testing image as an 8-bit SOA image.
  ImageSOA3(inputImage, red, green, blue);
  // Call the function we want to test.
  SOAImage outputImage = resizeImageSOA(inputImage, 2, 2);
  // Check parameters of resized image:
  EXPECT_EQ(outputImage.width, 2);
  EXPECT_EQ(outputImage.height, 2);
  EXPECT_EQ(outputImage.max_color_value, 255);
  // Check second pixel values, it should be (255, 0, 0)
  EXPECT_EQ(outputImage.red1_components[1], 255);  // Expected interpolated values.
  EXPECT_EQ(outputImage.green1_components[1], 0);
  EXPECT_EQ(outputImage.blue1_components[1], 0);
}


// Test resizeImageSOA from 4x4 to 2x2 for 6-byte pixel images
TEST(ResizeSOATests, ResizeLargeToSmall6) {
  // We define the 16-bit color component values for a 4x4 image.
  std::vector<uint16_t> red = {65535, 0, 0, 65535, 0, 65535, 0, 0, 65535, 0, 0, 65535, 65535, 0, 65535, 0};
  std::vector<uint16_t> green = {0, 65535, 0, 65535, 65535, 0, 65535, 0, 0, 65535, 0, 0, 65535, 0, 0, 65535};
  std::vector<uint16_t> blue = {0, 0, 65535, 0, 65535, 0, 65535, 0, 0, 0, 65535, 65535, 0, 65535, 0, 65535};
  SOAImage image; // Initialize testing image as a 16-bit SOA image.
  image.width = 4;
  image.height = 4;
  ImageSOA6(image, red, green, blue);
  // Call the resize function to reduce the image to 2x2.
  SOAImage outputImage = resizeImageSOA(image, 2, 2); // Actual output.
  // Check parameters of resized image.
  EXPECT_EQ(outputImage.width, 2);
  EXPECT_EQ(outputImage.height, 2);
  EXPECT_EQ(outputImage.max_color_value, 65535);
  // Check color values of the second resized pixel, it should be (65535, 65535, 0)
  EXPECT_EQ(outputImage.red2_components[1], 65535);
  EXPECT_EQ(outputImage.green2_components[1], 65535);
  EXPECT_EQ(outputImage.blue2_components[1], 0);
}


// Test resizeImageSOA from 2x2 to 4x4 for 3-byte pixel images
TEST(ResizeSOATests, ResizeSmallToLarge3) {
    // We define a 8-bit color component values for a 2x2 image.
    std::vector<uint8_t> red = {255, 0, 0, 255};
    std::vector<uint8_t> green = {0, 255, 255, 0};
    std::vector<uint8_t> blue = {0, 0, 255, 255};
    SOAImage image; // Initialize testing image as a 2x2 8-bit SOA image
    image.width = 2;
    image.height = 2;
    ImageSOA3(image, red, green, blue); // We define the testing image.
    // Call the resize function to increase the image to 4x4
    SOAImage outputImage = resizeImageSOA(image, 4, 4); // Actual output.
    // Check parameters of resized image.
    EXPECT_EQ(outputImage.width, 4);
    EXPECT_EQ(outputImage.height, 4);
    EXPECT_EQ(outputImage.max_color_value, 255);

    // Check color values of pixels of the resized image.
    EXPECT_EQ(outputImage.red1_components[0], 255);  // Top-left pixel should match top-left of the original 2x2.
    EXPECT_EQ(outputImage.green1_components[0], 0);
    EXPECT_EQ(outputImage.blue1_components[0], 0);

    EXPECT_EQ(outputImage.red1_components[3], 0);      // Top-right pixel should match top-right of the original 2x2.
    EXPECT_EQ(outputImage.green1_components[3], 255);
    EXPECT_EQ(outputImage.blue1_components[3], 0);

    EXPECT_EQ(outputImage.red1_components[12], 0);     // Bottom-left pixel should match bottom-left of the original 2x2.
    EXPECT_EQ(outputImage.green1_components[12], 255);
    EXPECT_EQ(outputImage.blue1_components[12], 255);

    EXPECT_EQ(outputImage.red1_components[15], 255); // Bottom-right pixel should match bottom-right of the original 2x2.
    EXPECT_EQ(outputImage.green1_components[15], 0);
    EXPECT_EQ(outputImage.blue1_components[15], 255);
}



// Test resizeImageSOA from 2x2 to 4x4 for 6-byte pixel images
TEST(ResizeSOATests, ResizeSmallToLarge6) {
    // We define a 16-bit color component values for a 2x2 image.
    std::vector<uint16_t> red = {65535, 0, 0, 65535};
    std::vector<uint16_t> green = {0, 65535, 65535, 0};
    std::vector<uint16_t> blue = {0, 0, 65535, 65535};
    SOAImage image; // Initialize testing image as a 2x2 16-bit SOA image.
    image.width = 2;
    image.height = 2;
    ImageSOA6(image, red, green, blue);
    // Call the resize function to increase the image to 4x4
    SOAImage outputImage = resizeImageSOA(image, 4, 4); // Actual output.
    // Check parameters of resized image
    EXPECT_EQ(outputImage.width, 4);
    EXPECT_EQ(outputImage.height, 4);
    EXPECT_EQ(outputImage.max_color_value, 65535);
    // Check color values of pixels of the resized image.
    EXPECT_EQ(outputImage.red2_components[0], 65535);  // Top-left pixel should match top-left of the original 2x2.
    EXPECT_EQ(outputImage.green2_components[0], 0);
    EXPECT_EQ(outputImage.blue2_components[0], 0);
    EXPECT_EQ(outputImage.red2_components[3], 0);      // Top-right pixel should match top-right of the original 2x2.
    EXPECT_EQ(outputImage.green2_components[3], 65535);
    EXPECT_EQ(outputImage.blue2_components[3], 0);
    EXPECT_EQ(outputImage.red2_components[12], 0);     // Bottom-left pixel should match bottom-left of the original 2x2.
    EXPECT_EQ(outputImage.green2_components[12], 65535);
    EXPECT_EQ(outputImage.blue2_components[12], 65535);
    EXPECT_EQ(outputImage.red2_components[15], 65535); // Bottom-right pixel should match bottom-right of the original 2x2.
    EXPECT_EQ(outputImage.green2_components[15], 0);
    EXPECT_EQ(outputImage.blue2_components[15], 65535);
}

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
// NOLINTEND(readability-magic-numbers)
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)