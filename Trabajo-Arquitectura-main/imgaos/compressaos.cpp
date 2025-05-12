#include "compressaos.hpp"
#include <vector>

// Writes the header of the compressed file, including basic image information
void write_header(std::ostream &output, const PPMImageAOS &image, size_t color_table_size) {
  output << "C6 " << image.width << " " << image.height << " "
         << image.max_color_value << " " << color_table_size << "\n";
}

// Processes an image with SmallPixel format, generates color table, and writes compressed data
void process_small_pixel_image(std::ostream &output, const PPMImageAOS &image) {
  std::vector <SmallPixel> unique_colors;
  auto color_map = generate_color_table(image.sPixels, unique_colors);

  write_header(output, image, unique_colors.size()); // Write header with color table size
  write_color_table(output, unique_colors);          // Write color table

  // Choose index size based on number of unique colors
  if (unique_colors.size() <= MAX_INDEX_SIZE_1B) {
    write_pixel_indices<SmallPixel, uint8_t>(output, image.sPixels, color_map);
  } else if (unique_colors.size() <= MAX_INDEX_SIZE_2B) {
    write_pixel_indices<SmallPixel, uint16_t>(output, image.sPixels, color_map);
  } else {
    throw std::runtime_error("Color table too large for SmallPixel format.");
  }
}

// Processes an image with LargePixel format, generates color table, and writes compressed data
void process_large_pixel_image(std::ostream &output, const PPMImageAOS &image) {
  std::vector <LargePixel> unique_colors;
  auto color_map = generate_color_table(image.lPixels, unique_colors);

  write_header(output, image, unique_colors.size()); // Write header with color table size
  write_color_table(output, unique_colors);          // Write color table

  // Choose index size based on number of unique colors
  if (unique_colors.size() <= MAX_INDEX_SIZE_1B) {
    write_pixel_indices<LargePixel, uint8_t>(output, image.lPixels, color_map);
  } else if (unique_colors.size() <= MAX_INDEX_SIZE_2B) {
    write_pixel_indices<LargePixel, uint16_t>(output, image.lPixels, color_map);
  } else if (unique_colors.size() <= MAX_INDEX_SIZE_4B) {
    write_pixel_indices<LargePixel, uint32_t>(output, image.lPixels, color_map);
  } else {
    throw std::runtime_error("Color table too large for LargePixel format.");
  }
}

// Main function to compress the image and write it in a custom compressed format
void write_cppm(const std::string &output_file, const PPMImageAOS &image) {
  std::ofstream output(output_file, std::ios::binary); // Open file in binary mode
  // Choose processing function based on pixel intensity
  if (image.max_color_value <= MAX_INTENSITY_FOR_1B) {
    process_small_pixel_image(output, image); // Process as SmallPixel image
  } else {
    process_large_pixel_image(output, image); // Process as LargePixel image
  }

  output.close(); // Close the output file
}