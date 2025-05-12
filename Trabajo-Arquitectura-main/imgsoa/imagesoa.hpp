#ifndef IMAGESOA_HPP
#define IMAGESOA_HPP

#include "../common/progargs.hpp"
#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <cmath>

const int MAX_INSTENSITY_1B = 255;
const int MAX_INSTENSITY = 65535;

// In SOA implementation we consider to separate the color components of each pixel into different vectors.
// This means we will have a structure composed of 3 vectors, one for red, one for green and one for blue.
// But, as we have to consider if the pixels are of 3 bytes or 6 bytes, we will have 6 vectors.

// SOA implementation for Image with pixels of 3 bytes (1 byte per color component):
struct SOAImage {
    int width;
    int height;
    int max_color_value;
    // Vectors if Image with pixels of 3 bytes (1 byte per color component):
    std::vector <uint8_t> red1_components; // vector to store values of the red components (each of 1 byte).
    std::vector <uint8_t> green1_components; // vector to store values of the green components (each of 1 byte).
    std::vector <uint8_t> blue1_components; // vector to store values of the blue components (each of 1 byte).
    // Vectors if Image with pixels of 6 bytes (2 bytes per color component):
    std::vector <uint16_t> red2_components; // vector to store values of the red components (each of 2 bytes).
    std::vector <uint16_t> green2_components; // vector to store values of the green components (each of 2 bytes).
    std::vector <uint16_t> blue2_components; // vector to store values of the blue components (each of 2 bytes).
};


// Function to read a PPM image from a file into SOA format:
auto readImageSOA(const std::string &filename) -> SOAImage;

// Function to write a PPM image (in SOA format) to a file:
void writeImageSOA(const std::string &filename, const SOAImage &image);

// Function to display metadata info. of the image:
void infoImageSOA(const std::string &filename);

//Main running function
void run_operationsoa(const ProgramArgs &args);


#endif //IMAGESOA_HPP
