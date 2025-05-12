#ifndef RESIZESOA_HPP
#define RESIZESOA_HPP

#include "imagesoa.hpp"
#include <unordered_map>
#include <queue>
#include <cmath>
#include <algorithm>

struct InterpolationCoords {
    int x_l, x_h, y_l, y_h; // Lower and upper bounds for x and y
    double x_weight, y_weight; // Interpolation weights for x and y
};

template<typename ComponentType>
struct AuxPixelVectsref {
    std::vector <ComponentType> red; // Reference to red component vector
    std::vector <ComponentType> green; // Reference to green component vector
    std::vector <ComponentType> blue; // Reference to blue component vector
};

template<typename T>
struct SOAPixelData {
    T r00, r10, r01, r11; // Red component values at 4 neighboring pixels
    T g00, g10, g01, g11; // Green component values at 4 neighboring pixels
    T b00, b10, b01, b11; // Blue component values at 4 neighboring pixels
};

struct InterpolationParams {
    size_t index; // Index in the output image
    int max_color_value; // Maximum color value (e.g., 255 for 8-bit)
};

struct ResizeParams {
    size_t x_prime; // Target x coordinate in the resized image
    size_t y_prime; // Target y coordinate in the resized image
    int newWidth; // Width of the resized image
    int newHeight; // Height of the resized image
};

template<typename T>
auto
fetchData(const SOAImage &inputImage, const InterpolationCoords &coords, const AuxPixelVectsref<T> &components) -> SOAPixelData<T> {
    SOAPixelData<T> data{};
    auto width = static_cast<size_t>(inputImage.width);
    auto x_l = static_cast<size_t>(coords.x_l);
    auto x_h = static_cast<size_t>(coords.x_h);
    auto y_l = static_cast<size_t>(coords.y_l);
    auto y_h = static_cast<size_t>(coords.y_h);

    data.r00 = components.red[(y_l * width) + x_l]; // Red component for top-left pixel
    data.r10 = components.red[(y_l * width) + x_h]; // Red component for top-right pixel
    data.r01 = components.red[(y_h * width) + x_l]; // Red component for bottom-left pixel
    data.r11 = components.red[(y_h * width) + x_h]; // Red component for bottom-right pixel

    data.g00 = components.green[(y_l * width) + x_l]; // Green component for top-left pixel
    data.g10 = components.green[(y_l * width) + x_h]; // Green component for top-right pixel
    data.g01 = components.green[(y_h * width) + x_l]; // Green component for bottom-left pixel
    data.g11 = components.green[(y_h * width) + x_h]; // Green component for bottom-right pixel

    data.b00 = components.blue[(y_l * width) + x_l]; // Blue component for top-left pixel
    data.b10 = components.blue[(y_l * width) + x_h]; // Blue component for top-right pixel
    data.b01 = components.blue[(y_h * width) + x_l]; // Blue component for bottom-left pixel
    data.b11 = components.blue[(y_h * width) + x_h]; // Blue component for bottom-right pixel

    return data;
}


template<typename T>
void interpolate(const SOAPixelData<T> &data, const InterpolationCoords &coords, AuxPixelVectsref<T> &outputComponents, const InterpolationParams &params) {
    double const red0 = (data.r00 * (1 - coords.x_weight)) + (data.r10 * coords.x_weight); // Interpolated red for top row
    double const red1 = (data.r01 * (1 - coords.x_weight)) + (data.r11 * coords.x_weight); // Interpolated red for bottom row
    outputComponents.red[params.index] = static_cast<T>(std::clamp(
            std::round((red0 * (1 - coords.y_weight)) + (red1 * coords.y_weight)), 0.0,
            static_cast<double>(params.max_color_value)));

    double const green0 = (data.g00 * (1 - coords.x_weight)) + (data.g10 * coords.x_weight); // Interpolated green for top row
    double const green1 = (data.g01 * (1 - coords.x_weight)) + (data.g11 * coords.x_weight); // Interpolated green for bottom row
    outputComponents.green[params.index] = static_cast<T>(std::clamp(
            std::round((green0 * (1 - coords.y_weight)) + (green1 * coords.y_weight)), 0.0,
            static_cast<double>(params.max_color_value)));

    double const blue0 = (data.b00 * (1 - coords.x_weight)) + (data.b10 * coords.x_weight); // Interpolated blue for top row
    double const blue1 = (data.b01 * (1 - coords.x_weight)) + (data.b11 * coords.x_weight); // Interpolated blue for bottom row
    outputComponents.blue[params.index] = static_cast<T>(std::clamp(
            std::round((blue0 * (1 - coords.y_weight)) + (blue1 * coords.y_weight)), 0.0,
            static_cast<double>(params.max_color_value)));
}

auto calculateCoords(const ResizeParams &params, const SOAImage &inputImage) -> InterpolationCoords;

auto resizeImageSOA(SOAImage &inputImage, int newWidth, int newHeight) -> SOAImage;

#endif //RESIZESOA_HPP
