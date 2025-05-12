#ifndef MAXLEVELSOA_HPP
#define MAXLEVELSOA_HPP

#include "imagesoa.hpp"
#include <cmath>
#include <stdexcept>

// Function to resize and recalculate components to 1 byte per component (3 bytes per pixel).
void resizeAndRecalculateToOneByte(SOAImage &newImage, int current_bytes_per_component, const SOAImage &image, int newMaxLevel);

// Function to resize and recalculate components to 2 bytes per component (6 bytes per pixel).
void resizeAndRecalculateToTwoBytes(SOAImage &newImage, int current_bytes_per_component, const SOAImage &image, int newMaxLevel);

// Main function that handles input validation and format change logic.
auto maxLevelImageSOA(const SOAImage &image, int newMaxLevel) -> SOAImage;

#endif //MAXLEVELSOA_HPP
