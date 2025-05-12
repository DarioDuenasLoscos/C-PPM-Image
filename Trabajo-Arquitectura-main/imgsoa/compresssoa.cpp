#include "compresssoa.hpp"
#include <ostream>
#include <fstream>
#include <string>
#include <stdexcept>

// Function to write the header of the file
void write_header(std::ostream &output, const SOAImage &image, size_t color_table_size) {
  output << "C6 " << image.width << " " << image.height << " "
         << image.max_color_value << " " << color_table_size << "\n"; // Write image metadata
}

// Function to process images with 1 byte per component
void process_small_pixel_image(std::ostream &output, const SOAImage &image) {
  AuxPixelVects<uint8_t> unique_colors;
  auto color_map = generate_color_table(image.red1_components, image.green1_components, image.blue1_components,
                                        unique_colors); // Generate color map

  write_header(output, image, unique_colors.red.size()); // Write file header
  write_color_table(output, unique_colors); // Write color table

  AuxPixelVects<uint8_t> const pixels_indexes{.red=image.red1_components, .green=image.green1_components,
                                              .blue=image.blue1_components}; // Initialize pixel indexes

  if (unique_colors.red.size() <= MAX_INDEX_SIZE_FOR_1B) {
    write_pixel_indices<uint8_t, uint8_t>(output, pixels_indexes, color_map); // Write 8-bit indices
  } else if (unique_colors.red.size() <= MAX_INDEX_SIZE_FOR_2B) {
    write_pixel_indices<uint8_t, uint16_t>(output, pixels_indexes, color_map); // Write 16-bit indices
  } else {
    throw std::runtime_error("Color table too large for 1-byte component format."); // Error if too many colors
  }
}

// Function to process images with 2 bytes per component
void process_large_pixel_image(std::ostream &output, const SOAImage &image) {
  AuxPixelVects<uint16_t> unique_colors;
  auto color_map = generate_color_table(image.red2_components, image.green2_components, image.blue2_components,
                                        unique_colors); // Generate color map

  write_header(output, image, unique_colors.red.size()); // Write file header
  write_color_table(output, unique_colors); // Write color table

  AuxPixelVects<uint16_t> const pixels_indexes{.red=image.red2_components, .green=image.green2_components,
                                               .blue=image.blue2_components}; // Initialize pixel indexes

  if (unique_colors.red.size() <= MAX_INDEX_SIZE_FOR_1B) {
    write_pixel_indices<uint16_t, uint8_t>(output, pixels_indexes, color_map); // Write 8-bit indices
  } else if (unique_colors.red.size() <= MAX_INDEX_SIZE_FOR_2B) {
    write_pixel_indices<uint16_t, uint16_t>(output, pixels_indexes, color_map); // Write 16-bit indices
  } else if (unique_colors.red.size() <= MAX_INDEX_SIZE_FOR_4B) {
    write_pixel_indices<uint16_t, uint32_t>(output, pixels_indexes, color_map); // Write 32-bit indices
  } else {
    throw std::runtime_error("Color table too large for 2-byte component format."); // Error if too many colors
  }
}

// Main function to write the image in C-PPM format
void write_cppm(const std::string &output_file, const SOAImage &image) {
  std::ofstream output(output_file, std::ios::binary); // Open output file in binary mode
  if (!output) {
    throw std::runtime_error("Error opening file for writing."); // Error if file can't be opened
  }

  if (image.max_color_value <= MAX_INSTENSITY_1B) {
    process_small_pixel_image(output, image); // Process 8-bit images
  } else {
    process_large_pixel_image(output, image); // Process 16-bit images
  }

  output.close(); // Close the file
}