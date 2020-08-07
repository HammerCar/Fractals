#pragma once

#include "opencl.h"
#include "Buffer.h"

namespace cl
{
	class Program;

	class Kernel
	{
	public:
		Kernel(const Program* program, const cl_kernel& kernel);
		~Kernel();

		void SetArgument(const int& index, const Buffer& buffer);
		void Run(const int& globalSize, const int& localSize = 0);

		explicit operator cl_kernel() const { return m_Kernel; };

	private:
		const Program* m_Program;

		cl_kernel m_Kernel;
	};
}
