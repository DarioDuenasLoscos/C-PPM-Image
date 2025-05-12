#include "maxlevelsoa.hpp"
#include <cmath>
#include <stdexcept>

// Function to resize and recalculate components to 1 byte per component (3 bytes per pixel).
void resizeAndRecalculateToOneByte(SOAImage &newImage, int current_bytes_per_component, const SOAImage &image, int newMaxLevel) {
  newImage.red1_components.resize(static_cast<size_t>(newImage.width) * static_cast<size_t>(newImage.height));
  newImage.green1_components.resize(static_cast<size_t>(newImage.width) * static_cast<size_t>(newImage.height));
  newImage.blue1_components.resize(static_cast<size_t>(newImage.width) * static_cast<size_t>(newImage.height));

  for (size_t i = 0; i < static_cast<size_t>(newImage.width) * static_cast<size_t>(newImage.height); ++i) {
    if (current_bytes_per_component == 1) {
      newImage.red1_components[i] = static_cast<uint8_t>(std::round(
          (image.red1_components[i] * newMaxLevel) / image.max_color_value));
      newImage.green1_components[i] = static_cast<uint8_t>(std::round(
          (image.green1_components[i] * newMaxLevel) / image.max_color_value));
      newImage.blue1_components[i] = static_cast<uint8_t>(std::round(
          (image.blue1_components[i] * newMaxLevel) / image.max_color_value));
    } else {
      newImage.red1_components[i] = static_cast<uint8_t>(std::round(
          (image.red2_components[i] * newMaxLevel) / image.max_color_value));
      newImage.green1_components[i] = static_cast<uint8_t>(std::round(
          (image.green2_components[i] * newMaxLevel) / image.max_color_value));
      newImage.blue1_components[i] = static_cast<uint8_t>(std::round(
          (image.blue2_components[i] * newMaxLevel) / image.max_color_value));
    }
  }
}

// Function to resize and recalculate components to 2 bytes per component (6 bytes per pixel).
void resizeAndRecalculateToTwoBytes(SOAImage &newImage, int current_bytes_per_component, const SOAImage &image,
                                    int newMaxLevel) {
  newImage.red2_components.resize(static_cast<size_t>(newImage.width) * static_cast<size_t>(newImage.height));
  newImage.green2_components.resize(static_cast<size_t>(newImage.width) * static_cast<size_t>(newImage.height));
  newImage.blue2_components.resize(static_cast<size_t>(newImage.width) * static_cast<size_t>(newImage.height));

  for (size_t i = 0; i < static_cast<size_t>(newImage.width) * static_cast<size_t>(newImage.height); ++i) {
    if (current_bytes_per_component == 1) {
      newImage.red2_components[i] = static_cast<uint16_t>(std::round(
          (image.red1_components[i] * newMaxLevel) / image.max_color_value));
      newImage.green2_components[i] = static_cast<uint16_t>(std::round(
          (image.green1_components[i] * newMaxLevel) / image.max_color_value));
      newImage.blue2_components[i] = static_cast<uint16_t>(std::round(
          (image.blue1_components[i] * newMaxLevel) / image.max_color_value));
    } else {
      newImage.red2_components[i] = static_cast<uint16_t>(std::round(
          (image.red2_components[i] * newMaxLevel) / image.max_color_value));
      newImage.green2_components[i] = static_cast<uint16_t>(std::round(
          (image.green2_components[i] * newMaxLevel) / image.max_color_value));
      newImage.blue2_components[i] = static_cast<uint16_t>(std::round(
          (image.blue2_components[i] * newMaxLevel) / image.max_color_value));
    }
  }
}

// Main function that handles input validation and format change logic.
auto maxLevelImageSOA(const SOAImage &image, int newMaxLevel) -> SOAImage {
  if (newMaxLevel <= 0 || newMaxLevel > MAX_INSTENSITY) { // Validate maximum value.
    throw std::invalid_argument("Maximum value not valid.");
  }

  SOAImage newImage; // Create new image in SOA format.
  newImage.width = image.width;
  newImage.height = image.height;
  newImage.max_color_value = newMaxLevel;

  // Determine current and new bytes per component.
  int const current_bytes_per_component = (image.max_color_value > MAX_INSTENSITY_1B) ? 2 : 1;
  int const new_bytes_per_component = (newMaxLevel > MAX_INSTENSITY_1B) ? 2 : 1;

  // Change format according to the new bytes per component size.
  if (new_bytes_per_component == 1) {
    resizeAndRecalculateToOneByte(newImage, current_bytes_per_component, image,  newMaxLevel);
  } else {
    resizeAndRecalculateToTwoBytes(newImage,current_bytes_per_component, image, newMaxLevel);
  }

  return newImage;
}