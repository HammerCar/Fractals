#include "Buffer.h"

#include "Program.h"


cl::Buffer::Buffer(const Program* program, const cl_mem& memObj, const int& size)
	: m_Program(program), m_MemoryObject(memObj), m_Size(size) {}

cl::Buffer::~Buffer()
{

}

void cl::Buffer::WriteData(const void* data, int size)
{
	if (size == 0)
		size = m_Size;

	CLCall(clEnqueueWriteBuffer(m_Program->GetCommandQueue(), m_MemoryObject, CL_TRUE, 0, size, data, 0, nullptr, nullptr));
}

void cl::Buffer::ReadData(void* data, int size)
{
	if (size == 0)
		size = m_Size;

	CLCall(clEnqueueReadBuffer(m_Program->GetCommandQueue(), m_MemoryObject, CL_TRUE, 0, size, data, 0, nullptr, nullptr));
}
