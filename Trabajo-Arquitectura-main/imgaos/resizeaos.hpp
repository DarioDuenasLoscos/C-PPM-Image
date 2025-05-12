#ifndef RESIZEAOS_HPP
#define RESIZEAOS_HPP

#include "imageaos.hpp" // Required for PPMImageAOS, SmallPixel, LargePixel
#include <cmath>        // Required for std::round
#include <algorithm>    // Required for std::clamp



// Template structure to hold image data information
template<typename PixelType>
struct ImageData {
    std::vector <PixelType> pixels;
    int width;
    int height;
    int max_color_value;
};

// Structure for interpolation data needed for resizing
template<typename PixelType>
struct InterpolationData {
    PixelType c00, c10, c01, c11; // Neighboring pixels for interpolation
    double x_weight;
    double y_weight;
    int max_color_value;
};

// Structure for holding image dimensions
struct ImageDataSize {
    int width;
    int height;
};

// Parameters for coordinate calculation during resizing
struct CoordinateParams {
    size_t x_prime;
    size_t y_prime;
    ImageDataSize inputSize;
    ImageDataSize outputSize;
};

// Holds calculated interpolation coordinates and weights
struct InterpolationCoords {
    int x_l, x_h;
    int y_l, y_h;
    double x_weight, y_weight;
};

// Interpolates a pixel based on neighboring pixels and weights
template<typename PixelType>
auto interpolatePixel(const InterpolationData<PixelType> &data) -> PixelType {
  PixelType resultPixel{};
  double const red0 = (data.c00.red * (1 - data.x_weight)) + (data.c10.red * data.x_weight);
  double const red1 = (data.c01.red * (1 - data.x_weight)) + (data.c11.red * data.x_weight);
  double const red = (red0 * (1 - data.y_weight)) + (red1 * data.y_weight);
  resultPixel.red = static_cast<decltype(PixelType::red) > (std::clamp(red, 0.0,
                                                                       static_cast<double>(data.max_color_value)));
  double const green0 = (data.c00.green * (1 - data.x_weight)) + (data.c10.green * data.x_weight);
  double const green1 = (data.c01.green * (1 - data.x_weight)) + (data.c11.green * data.x_weight);
  double const green = (green0 * (1 - data.y_weight)) + (green1 * data.y_weight);
  resultPixel.green = static_cast<decltype(PixelType::green) > (std::clamp(green, 0.0,
                                                                         static_cast<double>(data.max_color_value)));
  double const blue0 = (data.c00.blue * (1 - data.x_weight)) + (data.c10.blue * data.x_weight);
  double const blue1 = (data.c01.blue * (1 - data.x_weight)) + (data.c11.blue * data.x_weight);
  double const blue = (blue0 * (1 - data.y_weight)) + (blue1 * data.y_weight);
  resultPixel.blue = static_cast<decltype(PixelType::blue) > (std::clamp(blue, 0.0,
                                                                        static_cast<double>(data.max_color_value)));
  return resultPixel;
}

// Calculates coordinates and weights for interpolation during resizing
auto calculateCoords(const CoordinateParams &params) -> InterpolationCoords;

// Implementation of image resizing (math formula implementation)
template<typename PixelType>
void resizeImageAOS_impl(ImageData<PixelType> &inputImage, int inputwidth, ImageData<PixelType> &outputImage) {
  const ImageDataSize inputSize{inputwidth, inputImage.height};
  const ImageDataSize outputSize{outputImage.width, outputImage.height};
  const auto newWidth = static_cast<size_t>(outputImage.width);
  const auto newHeight = static_cast<size_t>(outputImage.height);
  outputImage.pixels.resize(newWidth * newHeight);
  const auto inputWidth = static_cast<size_t>(inputImage.width);
  const auto outputWidth = static_cast<size_t>(outputImage.width);
  for (size_t y_prime = 0; y_prime < newHeight; ++y_prime) {
    for (size_t x_prime = 0; x_prime < newWidth; ++x_prime) {
      CoordinateParams const params{.x_prime=x_prime, .y_prime=y_prime, .inputSize=inputSize, .outputSize=outputSize};
      InterpolationCoords coords = calculateCoords(params);
      size_t idx_00 = (static_cast<size_t>(coords.y_l) * inputWidth) + static_cast<size_t>(coords.x_l);
      size_t idx_10 = (static_cast<size_t>(coords.y_l) * inputWidth) + static_cast<size_t>(coords.x_h);
      size_t idx_01 = (static_cast<size_t>(coords.y_h) * inputWidth) + static_cast<size_t>(coords.x_l);
      size_t idx_11 = (static_cast<size_t>(coords.y_h) * inputWidth) + static_cast<size_t>(coords.x_h);
      PixelType c00 = inputImage.pixels[idx_00];
      PixelType c10 = inputImage.pixels[idx_10];
      PixelType c01 = inputImage.pixels[idx_01];
      PixelType c11 = inputImage.pixels[idx_11];
      InterpolationData<PixelType> data{c00, c10, c01, c11, coords.x_weight, coords.y_weight,
                                        inputImage.max_color_value};
      PixelType resultPixel = interpolatePixel(data);
      size_t idx_output = (y_prime * outputWidth) + x_prime;
      outputImage.pixels[idx_output] = resultPixel;
    }
  }
}

// Resizes a PPMImageAOS to a new width and height
auto resizeImageAOS(int newWidth, const PPMImageAOS& inputImage, int newHeight) -> PPMImageAOS;



#endif // RESIZEAOS_HPP