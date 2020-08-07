#include "Kernel.h"

#include "Program.h"


cl::Kernel::Kernel(const Program* program, const cl_kernel& kernel)
	: m_Program(program), m_Kernel(kernel) {}

cl::Kernel::~Kernel()
{

}

void cl::Kernel::SetArgument(const int& index, const Buffer& buffer)
{
	cl_mem memObj = (cl_mem)buffer;
	CLCall(clSetKernelArg(m_Kernel, index, sizeof(cl_mem), (void *)&memObj));
}

void cl::Kernel::Run(const int& globalSize, const int& localSize)
{
	size_t globalItemSize = globalSize;
	size_t localItemSize = localSize;

	size_t* localItemSizePtr = &localItemSize;
	if (localSize == 0)
		localItemSizePtr = nullptr;

	CLCall(clEnqueueNDRangeKernel(m_Program->GetCommandQueue(), m_Kernel, 1, nullptr, &globalItemSize, localItemSizePtr, 0, nullptr, nullptr));
}
