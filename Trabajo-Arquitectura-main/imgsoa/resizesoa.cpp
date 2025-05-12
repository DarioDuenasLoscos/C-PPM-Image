#include "resizesoa.hpp"
#include <cmath>


auto calculateCoords(const ResizeParams &params, const SOAImage &inputImage) -> InterpolationCoords {
  InterpolationCoords coords{};
  double const y_coord = static_cast<double>(params.y_prime) * (inputImage.height - 1) /
                         (params.newHeight - 1); // Calculate y position
  coords.y_l = static_cast<int>(std::floor(y_coord)); // Lower bound for y
  coords.y_h = static_cast<int>(std::ceil(y_coord)); // Upper bound for y
  coords.y_weight = y_coord - coords.y_l; // y weight for interpolation

  double const x_coord = static_cast<double>(params.x_prime) * (inputImage.width - 1) / (params.newWidth - 1); // Calculate x position
  coords.x_l = static_cast<int>(std::floor(x_coord)); // Lower bound for x
  coords.x_h = static_cast<int>(std::ceil(x_coord)); // Upper bound for x
  coords.x_weight = x_coord - coords.x_l; // x weight for interpolation

  coords.x_l = std::clamp(coords.x_l, 0, inputImage.width - 1); // Clamp x lower bound within image
  coords.x_h = std::clamp(coords.x_h, 0, inputImage.width - 1); // Clamp x upper bound within image
  coords.y_l = std::clamp(coords.y_l, 0, inputImage.height - 1); // Clamp y lower bound within image
  coords.y_h = std::clamp(coords.y_h, 0, inputImage.height - 1); // Clamp y upper bound within image

  return coords;
}


auto resizeImageSOA(SOAImage &inputImage, int newWidth, int newHeight) -> SOAImage {
  SOAImage outputImage;
  outputImage.width = newWidth;
  outputImage.height = newHeight;
  outputImage.max_color_value = inputImage.max_color_value;
  if (inputImage.max_color_value <= MAX_INSTENSITY_1B) { // Process 8-bit images
    outputImage.red1_components.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    outputImage.green1_components.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    outputImage.blue1_components.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    AuxPixelVectsref<uint8_t> const inputComponents{.red=inputImage.red1_components, .green=inputImage.green1_components,.blue=inputImage.blue1_components};
    AuxPixelVectsref<uint8_t> outputComponents{.red=outputImage.red1_components, .green=outputImage.green1_components,.blue=outputImage.blue1_components};
    for (size_t y_prime = 0; y_prime < static_cast<size_t>(newHeight); ++y_prime) {
      for (size_t x_prime = 0; x_prime < static_cast<size_t>(newWidth); ++x_prime) {
        ResizeParams const resizeParams{.x_prime=x_prime, .y_prime=y_prime, .newWidth=newWidth, .newHeight=newHeight}; // Initialize ResizeParams struct
        InterpolationCoords const coords = calculateCoords(resizeParams,inputImage); // Calculate interpolation coordinates
        auto data = fetchData<uint8_t>(inputImage, coords,inputComponents); // Fetch pixel data for interpolation
        InterpolationParams const interpParams{.index=((y_prime * static_cast<size_t>(newWidth)) + x_prime),.max_color_value=MAX_INSTENSITY_1B}; // Set interpolation parameters
        interpolate<uint8_t>(data, coords, outputComponents, interpParams); // Perform interpolation
      }
    }
  } else {
    outputImage.red2_components.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    outputImage.green2_components.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    outputImage.blue2_components.resize(static_cast<size_t>(newWidth) * static_cast<size_t>(newHeight));
    AuxPixelVectsref<uint16_t> const inputComponents{.red=inputImage.red2_components, .green=inputImage.green2_components,.blue=inputImage.blue2_components};
    AuxPixelVectsref<uint16_t> outputComponents{.red=outputImage.red2_components, .green=outputImage.green2_components,.blue=outputImage.blue2_components};
    for (size_t y_prime = 0; y_prime < static_cast<size_t>(newHeight); ++y_prime) {
      for (size_t x_prime = 0; x_prime < static_cast<size_t>(newWidth); ++x_prime) {
        ResizeParams const resizeParams{.x_prime=x_prime, .y_prime=y_prime, .newWidth=newWidth, .newHeight=newHeight};
        InterpolationCoords const coords = calculateCoords(resizeParams, inputImage);
        auto data = fetchData<uint16_t>(inputImage, coords, inputComponents);
        InterpolationParams const interpParams{.index=((y_prime * static_cast<size_t>(newWidth)) + x_prime), .max_color_value=inputImage.max_color_value};
        interpolate<uint16_t>(data, coords, outputComponents, interpParams);
      }
    }
  }
  return outputImage;
}