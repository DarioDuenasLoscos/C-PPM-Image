#include "imagesoa.hpp"
#include "../common/binaryio.hpp"
#include "maxlevelsoa.hpp"
#include "resizesoa.hpp"
#include "cutfreqsoa.hpp"
#include "compresssoa.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <stdexcept>

// readImageSOA function definition:
auto readImageSOA(const std::string &filename) -> SOAImage {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening the PPM file.");}
    std::string magic_number;
    file >> magic_number; // Extract the magic number for the file and store it in 'magic_number'.
    if (magic_number != "P6") {
        throw std::runtime_error("Unsupported PPM format.");}
    int width = 0;
    int height = 0;
    int max_color_value = 0;
    file >> width >> height >> max_color_value; // Extract the width, height and max. color of the file.
    file.get(); // Skip the newline character.
    SOAImage image;
    image.width = width;
    image.height = height;
    image.max_color_value = max_color_value;
    int const bytes_per_component = (max_color_value > MAX_INSTENSITY_1B) ? 2 : 1; // Determine how many bytes per color component (either 1 byte per color or 2).
    if (bytes_per_component == 1) { // If each Pixel is 3 bytes.
        image.red1_components.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        image.green1_components.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        image.blue1_components.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        for (size_t i = 0; i < static_cast<size_t>(width) *
                               static_cast<size_t>(height); ++i) { // Read pixel data from image and store each byte into its corresponding color vector:
            image.red1_components[i] = read_binary<uint8_t>(file);
            image.green1_components[i] = read_binary<uint8_t>(file);
            image.blue1_components[i] = read_binary<uint8_t>(file);}
    } else { // Each pixel is 6 bytes.
        image.red2_components.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        image.green2_components.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        image.blue2_components.resize(static_cast<size_t>(width) * static_cast<size_t>(height));
        for (size_t i = 0; i < static_cast<size_t>(width) *
                               static_cast<size_t>(height); ++i) { // Read pixel data from image and store each pair of bytes into its corresponding color vector:
            image.red2_components[i] = read_binary<uint16_t>(file);
            image.green2_components[i] = read_binary<uint16_t>(file);
            image.blue2_components[i] = read_binary<uint16_t>(file);}
    }
    file.close(); // close file.
    return image;
}

// writeImageSOA function definition:
void writeImageSOA(const std::string &filename, const SOAImage &image) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Error writing the PPM file.");
    }
    file << "P6\n";  // Write the "magic number" P6 in the file.
    file << image.width << " " << image.height << " "; // Write the width and height of the image in the file.
    file << image.max_color_value << "\n"; //  Write the max_color_value of the image in the file.
    int const bytes_per_component = (image.max_color_value > MAX_INSTENSITY_1B) ? 2
                                                                          : 1; // If max. color greater than 255, return 2.
    if (bytes_per_component == 1) { // Image with pixels of 3 bytes.
        for (size_t i = 0; i < static_cast<size_t>(image.width) *
                               static_cast<size_t>(image.height); ++i) { // Loop to go through each element of each vector.
            // Each element is the value for one of the color components of a pixel.
            write_binary<uint8_t>(file, image.red1_components[i]);
            write_binary<uint8_t>(file, image.green1_components[i]);
            write_binary<uint8_t>(file, image.blue1_components[i]);
        }
    } else { // Image with pixels of 2 bytes.
        for (size_t i = 0; i < static_cast<size_t>(image.width) * static_cast<size_t>(image.height); ++i) {
            write_binary<uint16_t>(file, image.red2_components[i]);
            write_binary<uint16_t>(file, image.green2_components[i]);
            write_binary<uint16_t>(file, image.blue2_components[i]);
        }
    }
    file.close();
}

// infoImageSOA function definition:
void infoImageSOA(const std::string &filename) {
    try {
        SOAImage const image = readImageSOA(filename); // Read the PPM file using the readImageSOA function.
        // If no errors encountered, we can display the metadata info. of the image:
        std::cout << "Metadata for image: " << filename << '\n';
        std::cout << "--------------------------------" << '\n';
        std::cout << "Width: " << image.width << " px" << '\n';
        std::cout << "Height: " << image.height << " px" << '\n';
        std::cout << "Max Color Value: " << image.max_color_value << '\n';
        // Determine pixel format based on the max. color value
        if (image.max_color_value <= MAX_INSTENSITY_1B) {
            std::cout << "Pixel Format: 3 bytes per pixel (8-bit color depth)" << '\n';
        } else if (image.max_color_value <= MAX_INSTENSITY) {
            std::cout << "Pixel Format: 6 bytes per pixel (16-bit color depth)" << '\n';
        } else {
            std::cout << "Pixel Format: Unsupported color depth" << '\n';
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}


void run_operationsoa(const ProgramArgs &args) {
    SOAImage const image = readImageSOA(args.input_file); // corresponding image in AOS format.
    if (args.operation == "info") {  // Perform 'info' operation
        infoImageSOA(args.input_file);
    } else if (args.operation == "compress") { // Perform 'compress' operation
        write_cppm(args.output_file, image);
    } else if (args.operation == "maxlevel") {
        SOAImage const new_image = maxLevelImageSOA(image, args.max_level); // Perform 'maxlevel' operation
        writeImageSOA(args.output_file, new_image);
    } else if (args.operation == "resize") {
        SOAImage r_image = readImageSOA(args.input_file);
        SOAImage const r_image_f = resizeImageSOA(r_image, args.width, args.height); // Perform 'resize' operation
        writeImageSOA(args.output_file, r_image_f);
    } else if (args.operation == "cutfreq") {
        SOAImage f_image = readImageSOA(args.input_file);
        removeLeastFrequentColors(f_image, args.max_level); // Perform 'cutfreq' operation
        writeImageSOA(args.output_file, f_image);
    }
}





