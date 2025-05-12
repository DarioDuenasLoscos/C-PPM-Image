#include "cutfreqsoa.hpp"

void removeLeastFrequentColors(SOAImage &image, int num_colors_to_remove) {
  if (image.max_color_value <= MAX_INSTENSITY_1B) { // Process 8-bit colors
    removeColors<uint8_t, uint32_t>(num_colors_to_remove,
                                    image.red1_components, image.green1_components, image.blue1_components);
  } else { // Process 16-bit colors
    removeColors<uint16_t, uint64_t>(num_colors_to_remove,
                                     image.red2_components, image.green2_components, image.blue2_components);
  }
}