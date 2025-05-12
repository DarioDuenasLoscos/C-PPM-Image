#include "imageaos.hpp"
#include "../common/binaryio.hpp"
#include "compressaos.hpp"
#include "maxlevelaos.hpp"
#include "resizeaos.hpp"
#include "cutfreqaos.hpp"
#include <fstream>
#include <iostream>


// readImageAOS function definition:
auto readImageAOS(const std::string &filename) -> PPMImageAOS {
  std::ifstream file(filename, std::ios::binary); // Open file in binary mode
  if (!file.is_open()) {
    throw std::runtime_error("Error opening the PPM file.");} // Throw error if file fails to open
  std::string magic_number;
  file >> magic_number; // Read magic number for format validation
  if (magic_number != "P6") {
    throw std::runtime_error("Unsupported PPM format.");} // Throw error if format is unsupported
  int width = 0; // NOLINT(misc-const-correctness) we have to do this because clang tidy says they have to be constants, but clearly they can't
  int height = 0; // NOLINT(misc-const-correctness)
  int max_color_value = 0; // NOLINT(misc-const-correctness)
  file >> width >> height >> max_color_value; // Extract width, height, and max color value
  if (width < 1 || height < 1 || max_color_value < 1) {
    throw std::runtime_error("Invalid image size.");} // Ensure dimensions are valid
  file.get(); // Skip newline character after header
  PPMImageAOS image;
  image.width = width;
  image.height = height;
  image.max_color_value = max_color_value;
  int const bytes_per_component = (max_color_value > MAX_INTENSITY_FOR_1B) ? 2 : 1; // Set bytes per color component
  size_t const total_pixels = static_cast<size_t>(width) * static_cast<size_t>(height);
  if (bytes_per_component == 1) { // NOLINT(bugprone-branch-clone) // Case for 1 byte per color component
    image.sPixels.resize(total_pixels);
    for (size_t i = 0; i < total_pixels; ++i) {
      SmallPixel sPixel = {.red=0, .green=0, .blue=0}; // Initialize pixel structure to zero
      sPixel.red = read_binary<uint8_t>(file);
      sPixel.green = read_binary<uint8_t>(file);
      sPixel.blue = read_binary<uint8_t>(file);
      image.sPixels[i] = sPixel;} // Store pixel in vector
  } else { // NOLINT(bugprone-branch-clone)  Case for 2 bytes per color component (noLint neccesary as they are not equal)
    image.lPixels.resize(total_pixels);
    for (size_t i = 0; i < total_pixels; ++i) {
      LargePixel lPixel = {.red=0, .green=0, .blue=0}; // Initialize large pixel structure to zero
      lPixel.red = read_binary<uint16_t>(file);
      lPixel.green = read_binary<uint16_t>(file);
      lPixel.blue = read_binary<uint16_t>(file);
      image.lPixels[i] = lPixel;} // Store large pixel in vector
  }
  file.close(); // Close file after reading
  return image;
}

// writeImageAOS function definition:
void writeImageAOS(const std::string &filename, const PPMImageAOS &image) {
  std::ofstream file(filename, std::ios::binary); // Open file for binary write
  if (!file.is_open()) {
    throw std::runtime_error("Error writing the PPM file."); // Error if file fails to open
  }
  file << "P6\n"; // Write P6 format magic number
  file << image.width << " " << image.height << " " << image.max_color_value << "\n"; // Write image header
  int const bytes_per_component = (image.max_color_value > MAX_INTENSITY_FOR_1B) ? 2 : 1; // Determine bytes per component
  if (bytes_per_component == 1) { // NOLINT(bugprone-branch-clone) // Case for 1 byte per component
    for (const SmallPixel &pixel: image.sPixels) { // Loop through each pixel
      write_binary<uint8_t>(file, pixel.red);
      write_binary<uint8_t>(file, pixel.green);
      write_binary<uint8_t>(file, pixel.blue);
    }
  } else { // NOLINT(bugprone-branch-clone) // Case for 2 bytes per component
    for (const LargePixel &pixel: image.lPixels) {
      write_binary<uint16_t>(file, pixel.red);
      write_binary<uint16_t>(file, pixel.green);
      write_binary<uint16_t>(file, pixel.blue);
    }
  }
  file.flush(); // Ensure all data is written
  file.close();
}

// infoImageAOS function definition:
void infoImageAOS(const std::string &filename) {
  try {
    PPMImageAOS const image = readImageAOS(filename); // Read image file
    // Display metadata information
    std::cout << "Metadata for image: " << filename << '\n';
    std::cout << "--------------------------------" << '\n';
    std::cout << "Width: " << image.width << " px" << '\n';
    std::cout << "Height: " << image.height << " px" << '\n';
    std::cout << "Max Color Value: " << image.max_color_value << '\n';
    if (image.max_color_value <= MAX_INTENSITY_FOR_1B) { // Check color depth for 8-bit format
      std::cout << "Pixel Format: 3 bytes per pixel (8-bit color depth)" << '\n';
    } else if (image.max_color_value < MAX_INTENSITY_AOS) { // Check color depth for 16-bit format
      std::cout << "Pixel Format: 6 bytes per pixel (16-bit color depth)" << '\n';
    } else {
      std::cout << "Pixel Format: Unsupported color depth" << '\n';
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n'; // Display error message if exception occurs
  }
}

// run_operation function definition:
void run_operationaos(const ProgramArgs &args) {
  if (args.operation == "info") {
    infoImageAOS(args.input_file); // Show image info
  } else if (args.operation == "maxlevel") {
    PPMImageAOS const new_image = maxLevelImageAOS(args.input_file, args.max_level); // Adjust max color level
    writeImageAOS(args.output_file, new_image); // Write modified image
  } else if (args.operation == "compress") {
    PPMImageAOS const c_image = readImageAOS(args.input_file); // Read image for compression
    write_cppm(args.output_file, c_image); // Write compressed image in CPPM format
  } else if (args.operation == "resize") {
    PPMImageAOS const r_image = readImageAOS(args.input_file); // Read image for resizing
    auto r_image_f= resizeImageAOS(args.width, r_image, args.height);  // Resize image
    writeImageAOS(args.output_file, r_image); // Write resized image
  } else if (args.operation == "cutfreq") {
    PPMImageAOS f_image = readImageAOS(args.input_file); // Read image for frequency cut
    removeLeastFrequentColors(f_image, args.max_level);
    writeImageAOS(args.output_file, f_image);
  }
}