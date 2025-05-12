#ifndef COMPRESSSOA_HPP
#define COMPRESSSOA_HPP

#include "imagesoa.hpp"
#include "../common/binaryio.hpp"
#include <vector>
#include <map>
#include <tuple>
#include <ostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cstdint>

const int MAX_INDEX_SIZE_FOR_1B = 256;
const int MAX_INDEX_SIZE_FOR_2B = 65536;
const long long MAX_INDEX_SIZE_FOR_4B = 4294967296;

template<typename t>
struct pixelcolors { //clang tidy doesn't allow to pass 3 "same type" values one after the other
    t red;
    t green;
    t blue;
};

// Structure to store RGB component vectors
template<typename ComponentType>
struct AuxPixelVects {
    std::vector <ComponentType> red; // Vector of red components
    std::vector <ComponentType> green; // Vector of green components
    std::vector <ComponentType> blue; // Vector of blue components
};

// Function to write the header of the file
void write_header(std::ostream &output, const SOAImage &image, size_t color_table_size);

// Function to write a single color in binary format
template<typename ComponentType>
void write_color(std::ostream &output, pixelcolors <ComponentType> pixel) {
    write_binary(output, pixel.red); // Write red component
    write_binary(output, pixel.green); // Write green component
    write_binary(output, pixel.blue); // Write blue component
}


// Function to write the entire color table
template<typename ComponentType>
void write_color_table(std::ostream &output, const AuxPixelVects<ComponentType> &unique_colors) {
    for (size_t i = 0; i < unique_colors.red.size(); ++i) {
    auto pixeltowrite = pixelcolors<ComponentType>{unique_colors.red[i], unique_colors.green[i], unique_colors.blue[i]};
    write_color(output, pixeltowrite); // Write each color
    }
}

// Function to generate an RGB color tuple
template<typename ComponentType>
auto get_color_tuple(
        const std::vector <ComponentType> &red, const std::vector <ComponentType> &green,
        const std::vector <ComponentType> &blue, size_t index) -> std::tuple <ComponentType, ComponentType, ComponentType> {
    return std::make_tuple(red[index], green[index], blue[index]); // Create RGB tuple
}

// Function to generate the color table
template<typename ComponentType>
auto
generate_color_table(const std::vector <ComponentType> &red, const std::vector <ComponentType> &green,
                     const std::vector <ComponentType> &blue, AuxPixelVects<ComponentType> &unique_colors) -> std::map <std::tuple<ComponentType, ComponentType, ComponentType>, size_t> {
    std::map <std::tuple<ComponentType, ComponentType, ComponentType>, size_t> color_map;
    for (size_t i = 0; i < red.size(); ++i) {
        auto color = get_color_tuple(red, green, blue, i); // Generate RGB tuple
        if (color_map.find(color) == color_map.end()) { // Check if color is unique
            color_map[color] = unique_colors.red.size(); // Map color to index
            unique_colors.red.push_back(red[i]); // Store red component
            unique_colors.green.push_back(green[i]); // Store green component
            unique_colors.blue.push_back(blue[i]); // Store blue component
        }
    }
    return color_map;
}

// Function to write pixel indices
template<typename ComponentType, typename IndexType>
void write_pixel_indices(std::ostream &output, const AuxPixelVects<ComponentType> &pixels_indexes,
                         const std::map <std::tuple<ComponentType, ComponentType, ComponentType>, size_t> &color_map) {
    for (size_t i = 0; i < pixels_indexes.red.size(); ++i) {
        auto color = get_color_tuple(pixels_indexes.red, pixels_indexes.green, pixels_indexes.blue,
                                     i); // Get color tuple
        auto index = static_cast<IndexType>(color_map.at(color)); // Get color index
        write_binary(output, index); // Write color index in binary format
    }
}

// Function to process images with 1 byte per component
void process_small_pixel_image(std::ostream &output, const SOAImage &image);

// Function to process images with 2 bytes per component
void process_large_pixel_image(std::ostream &output, const SOAImage &image);

// Main function to write the image in C-PPM format
void write_cppm(const std::string &output_file, const SOAImage &image);

#endif //COMPRESSSOA_HPP
