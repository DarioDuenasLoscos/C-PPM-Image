#ifndef MAXLEVELAOS_HPP
#define MAXLEVELAOS_HPP

#include "imageaos.hpp"
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

const int MAX_INTENSITY_AOS = 65536;

void scaleSmallToLarge(const PPMImageAOS &image, PPMImageAOS &scaled_image, int newMaxLevel);
void scaleLargeToSmall(const PPMImageAOS &image, PPMImageAOS &scaled_image, int newMaxLevel);
void scaleSmallToSmall(const PPMImageAOS &image, PPMImageAOS &scaled_image, int newMaxLevel);
void scaleLargeToLarge(const PPMImageAOS &image, PPMImageAOS &scaled_image, int newMaxLevel);
auto maxLevelImageAOS(const std::string &filename, int newMaxLevel) -> PPMImageAOS;

#endif // MAXLEVELAOS_HPP
