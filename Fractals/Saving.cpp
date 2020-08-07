#include "Saving.h"

#include <fstream>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

#include "windows.h"


void SavePPM(unsigned char* pixels, const std::string& fileName, const unsigned int& width)
{
	// Draws a PPM image using color data from iterate()
	std::ofstream imageFile(fileName, std::ios::trunc);
	std::string pixel_value;

	if (imageFile) {

		imageFile << "P3\n" << width << " " << width << "\n" << "255\n";

		for (unsigned int i = 0; i < width * width; i++)
		{
			pixel_value = std::to_string(pixels[i]);
			imageFile << pixel_value << " " << pixel_value << " " << pixel_value << "\n";
		}

		imageFile.close();
	}
	else
	{
		std::cout << "Error trying to write to file \"" << fileName << "\"" << std::endl;
	}
}



void SavePNG(unsigned char* data, const std::string& fileName, const unsigned int& width)
{
	std::cout << "Save png to " << fileName << std::endl;

	char bit_depth = 8; // 8 bits per color
	char color_type = PNG_COLOR_TYPE_GRAY; //PNG_COLOR_TYPE_RGB;

	// Create an array of pointers pointing to the start of each row
	png_bytep* row_pointers = new png_bytep[width]; // width == height
	for (unsigned int i = 0; i < width; i++) // width == height
	{
		row_pointers[i] = (png_bytep)(data + i * width);
	}

	/* create folder */
	char dirName[] = "images";
	wchar_t wName[20];
#pragma warning(suppress: 4996)
	mbstowcs(wName, dirName, strlen(dirName) + 1);
	CreateDirectory(wName, NULL);

	/* create file */
	FILE *fp;// = fopen(fileName.c_str(), "wb");
	fopen_s(&fp, fileName.c_str(), "wb");
	if (!fp)
	{
		std::cout << "[write_png_file] File " << fileName << " could not be opened for writing" << std::endl;
		return;
	}


	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
	{
		std::cout << "[write_png_file] png_create_write_struct failed" << std::endl;
		return;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		std::cout << "[write_png_file] png_create_info_struct failed" << std::endl;
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		std::cout << "[write_png_file] Error during init_io" << std::endl;
		return;
	}

	png_init_io(png_ptr, fp);


	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		std::cout << "[write_png_file] Error during writing header" << std::endl;
		return;
	}

	png_set_IHDR(png_ptr, info_ptr, width, width,
		bit_depth, color_type, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);


	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		std::cout << "[write_png_file] Error during writing bytes" << std::endl;
		return;
	}

	png_write_image(png_ptr, row_pointers);


	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		std::cout << "[write_png_file] Error during end of write" << std::endl;
		return;
	}

	png_write_end(png_ptr, NULL);

	/* cleanup heap allocation */
	delete[] row_pointers;

	fclose(fp);
}