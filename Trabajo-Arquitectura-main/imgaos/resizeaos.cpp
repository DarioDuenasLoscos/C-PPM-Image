#include "resizeaos.hpp"
#include <cmath>
#include <algorithm>

// Calculates coordinates and weights for interpolation during resizing
auto calculateCoords(const CoordinateParams &params) -> InterpolationCoords { // Calculate interpolation coordinates
  InterpolationCoords coords{}; // Initialize coords structure
  double const x_ratio = static_cast<double>(params.inputSize.width - 1) / (params.outputSize.width - 1); // X scaling ratio
  double const y_ratio = static_cast<double>(params.inputSize.height - 1) / (params.outputSize.height - 1); // Y scaling ratio
  double const x_coord = static_cast<double>(params.x_prime) * x_ratio; // X coordinate in input space
  double const y_coord = static_cast<double>(params.y_prime) * y_ratio; // Y coordinate in input space
  coords.x_l = static_cast<int>(std::floor(x_coord)); // Lower bound of x
  coords.x_h = static_cast<int>(std::ceil(x_coord)); // Upper bound of x
  coords.y_l = static_cast<int>(std::floor(y_coord)); // Lower bound of y
  coords.y_h = static_cast<int>(std::ceil(y_coord)); // Upper bound of y
  coords.x_weight = x_coord - coords.x_l; // X interpolation weight
  coords.y_weight = y_coord - coords.y_l; // Y interpolation weight
  coords.x_l = std::clamp(coords.x_l, 0, params.inputSize.width - 1); // Clamp x lower bound
  coords.x_h = std::clamp(coords.x_h, 0, params.inputSize.width - 1); // Clamp x upper bound
  coords.y_l = std::clamp(coords.y_l, 0, params.inputSize.height - 1); // Clamp y lower bound
  coords.y_h = std::clamp(coords.y_h, 0, params.inputSize.height - 1); // Clamp y upper bound
  return coords; // Return computed interpolation coordinates
}


// Resizes a PPMImageAOS to a new width and height
// Resizes a PPMImageAOS to new width and height (main function)
auto resizeImageAOS(int newWidth, const PPMImageAOS& inputImage, int newHeight) -> PPMImageAOS {
  PPMImageAOS outputImage;
  outputImage.width = newWidth;
  outputImage.height = newHeight;
  outputImage.max_color_value = inputImage.max_color_value;
  if (inputImage.max_color_value <= MAX_INTENSITY_FOR_1B) {
    ImageData<SmallPixel> inputData = {
        .pixels=inputImage.sPixels,
        .width=inputImage.width,
        .height=inputImage.height,
        .max_color_value=inputImage.max_color_value};
    ImageData<SmallPixel> outputData{
        .pixels=outputImage.sPixels,
        .width=outputImage.width,
        .height=outputImage.height,
        .max_color_value=outputImage.max_color_value};
    resizeImageAOS_impl(inputData, inputData.width, outputData);
  } else {
    ImageData<LargePixel> inputData{
        .pixels=inputImage.lPixels,
        .width=inputImage.width,
        .height=inputImage.height,
        .max_color_value=inputImage.max_color_value};
    ImageData<LargePixel> outputData{
        .pixels=outputImage.lPixels,
        .width=outputImage.width,
        .height=outputImage.height,
        .max_color_value=outputImage.max_color_value};
    resizeImageAOS_impl(inputData, inputData.width, outputData);
  }
  return outputImage;
}