#pragma once

#include <string>
#include <unordered_map>

#include "opencl.h"
#include "Kernel.h"
#include "Buffer.h"


namespace cl
{
	class Program
	{
	public:
		Program(const std::string& source);
		~Program();

		Kernel CreateKernel(const std::string& functionName);

		Buffer CreateInputBuffer(const int& size);
		Buffer CreateInputOutputBuffer(const int& size);
		Buffer CreateOutputBuffer(const int& size);


		inline cl_command_queue GetCommandQueue() const { return m_CommandQueue; };

	private:
		cl_device_id m_DeviceId = nullptr;
		cl_context m_Context = nullptr;
		cl_command_queue m_CommandQueue = nullptr;
		cl_program m_Program = nullptr;

		std::vector<Kernel> m_Kernels;
		std::vector<Buffer> m_Buffers;


		void CompileProgram(const std::string& source);
	};
}

