#include "maxlevelaos.hpp"
#include <cmath>

// Scales an image from SmallPixel to LargePixel format.
void scaleSmallToLarge(const PPMImageAOS &image, PPMImageAOS &scaled_image, int newMaxLevel) {
  scaled_image.lPixels.resize(static_cast<size_t>(scaled_image.width) * static_cast<size_t>(scaled_image.height));
  for (size_t i = 0; i < scaled_image.lPixels.size(); ++i) {
    SmallPixel const pixel = image.sPixels[i];
    scaled_image.lPixels[i].red = static_cast<uint16_t>(std::round(
        (pixel.red * newMaxLevel) / image.max_color_value));
    scaled_image.lPixels[i].green = static_cast<uint16_t>(std::round(
        (pixel.green * newMaxLevel) / image.max_color_value));
    scaled_image.lPixels[i].blue = static_cast<uint16_t>(std::round(
        (pixel.blue * newMaxLevel) / image.max_color_value));
  }
}

// Scales an image from LargePixel to SmallPixel format.
void scaleLargeToSmall(const PPMImageAOS &image, PPMImageAOS &scaled_image, int newMaxLevel) {
  scaled_image.sPixels.resize(static_cast<size_t>(scaled_image.width) * static_cast<size_t>(scaled_image.height));
  for (size_t i = 0; i < scaled_image.sPixels.size(); ++i) {
    LargePixel const pixel = image.lPixels[i];
    scaled_image.sPixels[i].red = static_cast<uint8_t>(std::round(
        (pixel.red * newMaxLevel) / image.max_color_value));
    scaled_image.sPixels[i].green = static_cast<uint8_t>(std::round(
        (pixel.green * newMaxLevel) / image.max_color_value));
    scaled_image.sPixels[i].blue = static_cast<uint8_t>(std::round(
        (pixel.blue * newMaxLevel) / image.max_color_value));
  }
}

// Scales an image from SmallPixel to SmallPixel (only changes intensity range).
void scaleSmallToSmall(const PPMImageAOS &image, PPMImageAOS &scaled_image, int newMaxLevel) {
  scaled_image.sPixels.resize(static_cast<size_t>(scaled_image.width) * static_cast<size_t>(scaled_image.height));
  for (size_t i = 0; i < scaled_image.sPixels.size(); ++i) {
    SmallPixel const pixel = image.sPixels[i];
    scaled_image.sPixels[i].red = static_cast<uint8_t>(std::round(
        (pixel.red * newMaxLevel) / image.max_color_value));
    scaled_image.sPixels[i].green = static_cast<uint8_t>(std::round(
        (pixel.green * newMaxLevel) / image.max_color_value));
    scaled_image.sPixels[i].blue = static_cast<uint8_t>(std::round(
        (pixel.blue * newMaxLevel) / image.max_color_value));
  }
}

// Scales an image from LargePixel to LargePixel (only changes intensity range).
void scaleLargeToLarge(const PPMImageAOS &image, PPMImageAOS &scaled_image, int newMaxLevel) {
  scaled_image.lPixels.resize(static_cast<size_t>(scaled_image.width) * static_cast<size_t>(scaled_image.height));
  for (size_t i = 0; i < scaled_image.lPixels.size(); ++i) {
    LargePixel const pixel = image.lPixels[i];
    scaled_image.lPixels[i].red = static_cast<uint16_t>(std::round(
        (pixel.red * newMaxLevel) / image.max_color_value));
    scaled_image.lPixels[i].green = static_cast<uint16_t>(std::round(
        (pixel.green * newMaxLevel) / image.max_color_value));
    scaled_image.lPixels[i].blue = static_cast<uint16_t>(std::round(
        (pixel.blue * newMaxLevel) / image.max_color_value));
  }
}

// Adjusts the maximum color level of an image based on the new max level.
auto maxLevelImageAOS(const std::string &filename, int newMaxLevel) -> PPMImageAOS {
  if (newMaxLevel <= 0 || newMaxLevel > MAX_INTENSITY_AOS) { // Validate new max level range
    throw std::invalid_argument("Maximum value not valid.");
  }

  PPMImageAOS const image = readImageAOS(filename); // Read the input image
  PPMImageAOS scaled_image;
  scaled_image.width = image.width;
  scaled_image.height = image.height;
  scaled_image.max_color_value = newMaxLevel; // Set new max color value for scaled image

  bool const isNewSmallPixel = (newMaxLevel <= MAX_INTENSITY_FOR_1B); // Check if new max level is for SmallPixel
  bool const isOldSmallPixel = (image.max_color_value <=
                          MAX_INTENSITY_FOR_1B); // Check if original max level is for SmallPixel

  // Scale image based on original and target pixel formats
  if (isOldSmallPixel && !isNewSmallPixel) {
    scaleSmallToLarge(image, scaled_image, newMaxLevel);
  } else if (!isOldSmallPixel && isNewSmallPixel) {
    scaleLargeToSmall(image, scaled_image, newMaxLevel);
  } else if (isNewSmallPixel) {
    scaleSmallToSmall(image, scaled_image, newMaxLevel);
  } else {
    scaleLargeToLarge(image, scaled_image, newMaxLevel);
  }

  return scaled_image; // Return the adjusted image
}
