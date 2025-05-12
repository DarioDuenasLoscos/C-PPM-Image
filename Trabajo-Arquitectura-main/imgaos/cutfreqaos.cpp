#include "cutfreqaos.hpp"

// Main function to remove the least frequent colors from the image
void removeLeastFrequentColors(PPMImageAOS &image, int num_colors_to_remove) {
  size_t const total_pixels = static_cast<size_t>(image.width) * static_cast<size_t>(image.height);

  if (static_cast<size_t>(num_colors_to_remove) >= total_pixels) {
    // Set all pixels to black (0,0,0)
    if (image.max_color_value <= MAX_INTENSITY_FOR_1B) {
      for (auto &pixel: image.sPixels) {
        pixel.red = 0;
        pixel.green = 0;
        pixel.blue = 0;
      }
    } else {
      for (auto &pixel: image.lPixels) {
        pixel.red = 0;
        pixel.green = 0;
        pixel.blue = 0;
      }
    }
  } else {
    // Optimized processing using KD-Tree without explicit new/delete
    if (image.max_color_value <= MAX_INTENSITY_FOR_1B) {
      removeColors<SmallPixel>(num_colors_to_remove, image.sPixels);
    } else {
      removeColors<LargePixel>(num_colors_to_remove, image.lPixels);
    }
  }
}
