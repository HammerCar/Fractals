#pragma once

#include <string>

void SavePPM(unsigned char* pixels, const std::string& fileName, const unsigned int& width);
void SavePNG(unsigned char* data, const std::string& fileName, const unsigned int& width);
