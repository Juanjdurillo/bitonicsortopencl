
// Multiply two matrices A * B = C

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <CL/cl.h>

#define SIZE 16


/////////////////////////////////////////////////////////
// Program main
/////////////////////////////////////////////////////////

/*printf("CL_INVALID_CONTEXT %d\n", CL_INVALID_CONTEXT);
printf("CL_INVALID_VALUE %d\n", CL_INVALID_VALUE);
printf("CL_OUT_OF_HOST_MEMORY %d\n", CL_OUT_OF_HOST_MEMORY);
printf("Number of platforms: %d\n", numPlatforms);*/
// the second call, get the platforms


int main(int argc, char** argv)
{

	
	// allocate and initialize host memory for matrices A and B
	unsigned int mem_size_A = sizeof(float)* SIZE;
	float* h_A = (float*)malloc(mem_size_A);

	h_A[0] = 15;
	h_A[1] = 15;
	h_A[2] = 7;
	h_A[3] = 13;
	h_A[4] = 121;
	h_A[5] = 0;
	h_A[6] = 1;
	h_A[7] = 27;
	h_A[8] = 32;
	h_A[9] = 45;
	h_A[10] = 44;
	h_A[11] = 77;
	h_A[12] = 99;
	h_A[13] = 21;
	h_A[14] = 32;
	h_A[15] = 133;
	

	printf("%f \n", h_A[15]);
	
	// 5. Initialize OpenCL specific variables
	cl_program clProgram;
	cl_kernel clKernel;

	size_t dataBytes;
	size_t kernelLength;
	cl_int errcode;

	// OpenCL device memory for matrices
	cl_mem d_A;
	
	//--------------retrieving platforms-----------------------//	
	// the number of platforms is retrieved by using a first call
	// to clGetPlatformsIDs() with NULL argument as second argument
	cl_int status;
	cl_uint numPlatforms;
	cl_platform_id *platforms = NULL;

	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	printf("%d \n", status);
	printf("%d \n", status);
	printf("%d \n", status);
	printf("%d \n", status);
	printf("%d \n", status);


	platforms = (cl_platform_id *)malloc(numPlatforms * sizeof(cl_platform_id));
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);


	//--------------------------------------------------------//
	//-------------retrieving devices ------------------------//
	cl_uint numDevices;
	cl_device_id *devices = NULL;

	// as before, the devices are retrieved in two steps
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	devices = (cl_device_id *)malloc(numDevices * sizeof(cl_device_id));
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);

	cl_context context = NULL;
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);

	//--------------------------------------------------------//
	//-------------creating a coMmand queue -------------------//
	cl_command_queue cmdQueue;
	cmdQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);

	//Create a command-queue
	if (status != 0) {
		printf("Error: %s\n", errcode);
		exit(errcode);
	}
	// Setup device memory
	d_A = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, mem_size_A, h_A, &errcode);
	
	// 6. Load and build OpenCL kernel
	const char fileName[] = "bitonic_sort.cl";
	size_t sourceSize;
	char *source_str;
	FILE* fp = fopen(fileName, "rb");
	if (!fp) {
		printf("Error while loading the source code %s\n", fp);
		exit(1);
	}
	source_str = (char *)malloc(sizeof(char)*1000000);
	sourceSize = fread(source_str, 1, 1000000, fp);
	fclose(fp);



	clProgram = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t*)&sourceSize, &errcode);
	if (errcode != 0) {
		printf("Error: %d\n", errcode);
		exit(errcode);
	}
	errcode = clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);
	if (errcode != 0) {
		printf("Error: %d\n", errcode);
		exit(errcode);
	}
	clKernel = clCreateKernel(clProgram, "bitonic", &errcode);
	if (errcode != 0) {
		printf("Error: %d\n", errcode);
		exit(errcode);
	}

	size_t  globalWorkSize[2];
	cl_int n = 16;
	cl_int l = 4;
	errcode = clSetKernelArg(clKernel, 0, sizeof(cl_mem), (void *)&d_A);
	errcode |= clSetKernelArg(clKernel, 1, sizeof(int), (void *)&n);
	errcode |= clSetKernelArg(clKernel, 2, sizeof(int), (void *)&l);


	size_t localWorkSize[2];
	localWorkSize[0] = 8;
	localWorkSize[1] = 1;
	//size_t localWorkSize[2] = { 1024, 1 };*/
	//	size_t *localWorkSize = NULL;
	globalWorkSize[0] = 8;
	globalWorkSize[1] = 1;
	

	cl_event profiling_event;

	errcode = clEnqueueNDRangeKernel(cmdQueue, clKernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, &profiling_event);
	if (errcode != 0) {
		printf("Error: %s\n", errcode);
		exit(errcode);
	}
	clWaitForEvents(1, &profiling_event);

	cl_ulong start_time;
	cl_ulong finish_time;
	clGetEventProfilingInfo(profiling_event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
	clGetEventProfilingInfo(profiling_event, CL_PROFILING_COMMAND_END, sizeof(finish_time), &finish_time, NULL);


	cl_ulong total_time = finish_time - start_time;
	printf("\start time in nanoseconds = %lu\n", start_time);
	printf("\nend time in nanoseconds = %lu\n", finish_time);
	printf("\nAverage time in nanoseconds = %lu\n", total_time);
	if (errcode != 0) {
		printf("Error: %s\n", errcode);
		exit(errcode);
	}
	// 8. Retrieve result from device
	errcode = clEnqueueReadBuffer(cmdQueue, d_A, CL_TRUE, 0, mem_size_A, h_A, 0, NULL, NULL);
	if (errcode != 0) {
		printf("Error: %s\n", errcode);
		exit(errcode);
	}

	// 9. print out the results
	printf("\n\nMatrix A\n");
	for (unsigned i = 0; i < 16; i++)
	{
		printf("%f ", h_A[i]);
	
	}

	free(h_A);

	clReleaseMemObject(d_A);
	
	free(devices);
	free(platforms);

	clReleaseKernel(clKernel);
	clReleaseProgram(clProgram);
	clReleaseCommandQueue(cmdQueue);

	while (true) { ; }
}