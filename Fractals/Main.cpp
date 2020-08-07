#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "windows.h"

#include "cl/Program.h"
#include "Saving.h"


void Replace(std::string& str, const std::string& search, const std::string& replace)
{
	size_t index = 0;
	while (true) {
		index = str.find(search, index);
		if (index == std::string::npos) break;

		str.replace(index, search.size(), replace);

		index += replace.size();
	}
}

std::string ReadSource()
{
	std::ifstream stream("E:\\CodeProjects\\C++\\OpenCL\\Fractals\\calculate.cl");

	if (stream)
	{
		std::string source = "";

		std::string line;
		while (std::getline(stream, line))
		{
			source += line + "\n";
		}

		return source;
	}

	std::cout << "File not found" << std::endl;
	throw - 1;
	return "";
}

long GetTime()
{
	SYSTEMTIME time;
	GetSystemTime(&time);
	return (time.wMinute * 60 * 1000) + (time.wSecond * 1000) + time.wMilliseconds;
}

void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}



void RenderImage(cl::Kernel& calculateKernel, cl::Buffer& outputBuffer, const unsigned int& width, const std::string& fileName);

int main()
{
	std::cout << "Starting" << std::endl;

	ShowConsoleCursor(false);

	std::cout << std::fixed << std::setprecision(2);

	unsigned int width = 10000;
	double zoom = 8;
	double posX = -0.5;
	double posY = 0;
	unsigned int maxIterations = 100;


	std::string source = ReadSource();

	Replace(source, "WIDTH", std::to_string(width));
	Replace(source, "X_POS", std::to_string(posX));
	Replace(source, "Y_POS", std::to_string(posY));
	Replace(source, "ZOOM", std::to_string(zoom));
	Replace(source, "MAX_ITERATIONS", std::to_string(maxIterations));


	// Create Program
	cl::Program program(source);

	// Create Kernels
	cl::Kernel calculateKernel = program.CreateKernel("calculate");

	// Create Buffers
	cl::Buffer inputBuffer = program.CreateInputBuffer(2 * sizeof(double));
	cl::Buffer outputBuffer = program.CreateOutputBuffer(width * width * sizeof(char));

	// Set Arguments
	calculateKernel.SetArgument(0, inputBuffer);
	calculateKernel.SetArgument(1, outputBuffer);


	double parameters[2];

	int imageCountX = 5;
	int imageCountY = 5;

	for (int x = -(imageCountX / 2); x < imageCountX / 2 + 1; x++)
	{
		for (int y = -(imageCountY / 2); y < imageCountY / 2 + 1; y++)
		{
			parameters[0] = posX + x / (zoom / 4.0);
			parameters[1] = posY + y / (zoom / 4.0);
			inputBuffer.WriteData(parameters);

			RenderImage(calculateKernel, outputBuffer, width, "images/image" + std::to_string(x + imageCountX / 2) + "." + std::to_string(y + imageCountY / 2) + ".png");
		}
	}
}

void RenderImage(cl::Kernel& calculateKernel, cl::Buffer& outputBuffer, const unsigned int& width, const std::string& fileName)
{
	long timeMs = GetTime();

	std::cout << std::endl << "Calculating" << std::endl;

	calculateKernel.Run(width * width);

	std::cout << "Calculation took " << GetTime() - timeMs << " ms" << std::endl;
	timeMs = GetTime();

	std::cout << "Reading buffer" << std::endl;

	unsigned char* buffer = new unsigned char[width * width];
	outputBuffer.ReadData(buffer);

	std::cout << "Buffer read" << std::endl;

	//SavePPM(buffer, "image.ppm", width);
	SavePNG(buffer, fileName, width);

	std::cout << "Writing took " << GetTime() - timeMs << " ms" << std::endl;

	delete[] buffer;
}
