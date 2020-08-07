#pragma once

#include "opencl.h"


namespace cl
{
	class Program;

	class Buffer
	{
	public:
		Buffer(const Program* program, const cl_mem& memObj, const int& size);
		~Buffer();

		void WriteData(const void* data, int size = 0);
		void ReadData(void* data, int size = 0);


		inline int Size() const { return m_Size; };

		explicit operator cl_mem() const { return m_MemoryObject; };

	private:
		const Program* m_Program;

		const cl_mem m_MemoryObject;
		const int m_Size;
	};
}
