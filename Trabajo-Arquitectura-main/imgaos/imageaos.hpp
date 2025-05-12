#ifndef IMAGEAOS_HPP
#define IMAGEAOS_HPP

#include "../common/progargs.hpp"
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>

const int MAX_INTENSITY_FOR_1B = 255;
// In AOS implementation we consider the case where each pixel is represented as a structure, that stores its color values (RGB).
// Then, we will perform modifications on the image using the vector of these pixel structures.

// AOS implementation for 1 Pixel of 3 bytes (1 byte per color component):
struct SmallPixel {
    uint8_t red; // 1 byte for red.
    uint8_t green; // 1 byte for green.
    uint8_t blue; // 1 byte for blue.

    using ComponentType [[maybe_unused]] = uint8_t;

    // Operador de comparación necesario para std::map
    auto operator<(const SmallPixel &other) const -> bool {
      return std::tie(red, green, blue) < std::tie(other.red, other.green, other.blue);
    }

    auto operator==(const SmallPixel &other) const -> bool {
      return red == other.red && green == other.green && blue == other.blue;
    }
};

// AOS implementation for 1 Pixel of 6 bytes (2 bytes per color component):
struct LargePixel {
    uint16_t red; // 2 bytes for red.
    uint16_t green; // 2 bytes for green.
    uint16_t blue; // 2 bytes for blue.

    using ComponentType [[maybe_unused]] = uint16_t;

    auto operator<(const LargePixel &other) const -> bool {
      return std::tie(red, green, blue) < std::tie(other.red, other.green, other.blue);
    }

    auto operator==(const LargePixel &other) const -> bool {
      return red == other.red && green == other.green && blue == other.blue;
    }
};

// Structure to store the image with AOS format.
struct PPMImageAOS {
    int width;
    int height;
    int max_color_value;
    std::vector <SmallPixel> sPixels;  // Vector of Pixels of 3 bytes (SmallPixel).
    std::vector <LargePixel> lPixels;  // Vector of Pixels of 6 bytes (LargePixel).
};


// Function to read a PPM image from a file into AOS format:
auto readImageAOS(const std::string &filename) -> PPMImageAOS;

// Function to write a PPM image (in AOS format) to a file:
void writeImageAOS(const std::string &filename, const PPMImageAOS &image);

// Function to display metadata info. of the image:
void infoImageAOS(const std::string &filename);

void run_operationaos(const ProgramArgs &args);

#endif //IMAGEAOS_HPP
