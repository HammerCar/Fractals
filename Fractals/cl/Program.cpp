#include "Program.h"

cl::Program::Program(const std::string& source)
{
	cl_int ret;

	// Find device
	cl_uint retNumDevices;
	cl_uint retNumPlatforms;

	// Get platform count
	CLCall(clGetPlatformIDs(0, nullptr, &retNumPlatforms));

	// Get platforms
	cl_platform_id* platforms = new cl_platform_id[retNumPlatforms];
	CLCall(clGetPlatformIDs(retNumPlatforms, platforms, nullptr));

	// Get devices
	CLCall(clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 1, &m_DeviceId, &retNumDevices));
	delete[] platforms;

	// Create an OpenCL context
	m_Context = clCreateContext(nullptr, 1, &m_DeviceId, nullptr, nullptr, &ret);
	CLCheck;

	// Create a command queue
	m_CommandQueue = clCreateCommandQueue(m_Context, m_DeviceId, 0, &ret);
	CLCheck;

	CompileProgram(source);
}

cl::Program::~Program()
{
	CLCall(clFlush(m_CommandQueue));
	CLCall(clFinish(m_CommandQueue));

	for (Kernel& kernel : m_Kernels)
	{
		CLCall(clReleaseKernel((cl_kernel)kernel));
	}

	CLCall(clReleaseProgram(m_Program));

	for (Buffer& buffer : m_Buffers)
	{
		CLCall(clReleaseMemObject((cl_mem)buffer));
	}

	CLCall(clReleaseCommandQueue(m_CommandQueue));
	CLCall(clReleaseContext(m_Context));
}

cl::Kernel cl::Program::CreateKernel(const std::string& functionName)
{
	cl_int ret;

	cl_kernel clKernel = clCreateKernel(m_Program, functionName.c_str(), &ret);
	CLCheck;

	Kernel kernel(this, clKernel);
	m_Kernels.push_back(kernel);

	return kernel;
}

cl::Buffer cl::Program::CreateInputBuffer(const int& size)
{
	cl_int ret;
	cl_mem memObj = clCreateBuffer(m_Context, CL_MEM_READ_ONLY, size, nullptr, &ret);
	CLCheck;

	Buffer buffer(this, memObj, size);
	m_Buffers.push_back(buffer);

	return buffer;
}

cl::Buffer cl::Program::CreateInputOutputBuffer(const int& size)
{
	cl_int ret;
	cl_mem memObj = clCreateBuffer(m_Context, CL_MEM_READ_WRITE, size, nullptr, &ret);
	CLCheck;

	Buffer buffer(this, memObj, size);
	m_Buffers.push_back(buffer);

	return buffer;
}

cl::Buffer cl::Program::CreateOutputBuffer(const int& size)
{
	cl_int ret;
	cl_mem memObj = clCreateBuffer(m_Context, CL_MEM_WRITE_ONLY, size, nullptr, &ret);
	CLCheck;

	Buffer buffer(this, memObj, size);
	m_Buffers.push_back(buffer);

	return buffer;
}

void cl::Program::CompileProgram(const std::string& source)
{
	cl_int ret;

	const char *source_str = source.c_str();
	size_t source_size = source.size();

	// Create a program from the kernel source
	m_Program = clCreateProgramWithSource(m_Context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	CLCheck;

	// Build the program
	ret = clBuildProgram(m_Program, 1, &m_DeviceId, nullptr, nullptr, nullptr);

	if (ret != CL_SUCCESS)
	{
		size_t len;
		char buffer[204800];
		cl_build_status bldstatus;

		printf("\nError %d: Failed to build program executable [ %s ]\n", ret, getErrorString(ret));

		ret = clGetProgramBuildInfo(m_Program, m_DeviceId, CL_PROGRAM_BUILD_STATUS, sizeof(bldstatus), (void *)&bldstatus, &len);
		if (ret != CL_SUCCESS)
		{
			printf("Build Status error %d: %s\n", ret, getErrorString(ret));
			exit(1);
		}

		if (bldstatus == CL_BUILD_SUCCESS) printf("Build Status: CL_BUILD_SUCCESS\n");
		if (bldstatus == CL_BUILD_NONE) printf("Build Status: CL_BUILD_NONE\n");
		if (bldstatus == CL_BUILD_ERROR) printf("Build Status: CL_BUILD_ERROR\n");
		if (bldstatus == CL_BUILD_IN_PROGRESS) printf("Build Status: CL_BUILD_IN_PROGRESS\n");

		ret = clGetProgramBuildInfo(m_Program, m_DeviceId, CL_PROGRAM_BUILD_OPTIONS, sizeof(buffer), buffer, &len);
		if (ret != CL_SUCCESS)
		{
			printf("Build Options error %d: %s\n", ret, getErrorString(ret));
			exit(1);
		}

		printf("Build Options: %s\n", buffer);

		ret = clGetProgramBuildInfo(m_Program, m_DeviceId, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		if (ret != CL_SUCCESS)
		{
			printf("Build Log error %d: %s\n", ret, getErrorString(ret));
			exit(1);
		}

		printf("Build Log:\n%s\n", buffer);

		__debugbreak;
	}
}
