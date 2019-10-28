/**********************************************************************
Copyright 2015 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/

// For clarity,error checking has been omitted.
#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#define SUCCESS 0
#define FAILURE 1

using namespace std;

/* convert the kernel file into a string */
int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if(f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size+1];
		if(!str)
		{
			f.close();
			return 0;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}
	cout<<"Error: failed to open file\n:"<<filename<<endl;
	return FAILURE;
}

int* rndarr (int sz)
{
  int* v;
  int  i;

  v = (int*) malloc(sz * sizeof(int));

  for (i = 0; i < sz; i++)
    v[i] = rand() % sz;

  return v;
}
void prnarr (int* v, int sz)
{
  int i;

  for (i = 0; i < sz; i++)
    printf("%d ", v[i]);
  printf("\n");
}
int main(int argc, char* argv[])
{

	/*Step1: Getting platforms and choose an available one.*/
	cl_uint numPlatforms;	//the NO. of platforms
	cl_platform_id platform = NULL;	//the chosen platform
	cl_int	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	int input_size;

	if(argv[1] == NULL) input_size = 100;
	else
		input_size = atoi(argv[1]);

	if (status != CL_SUCCESS)
	{
		cout << "Error: Getting platforms!" << endl;
		return FAILURE;
	}

	/*For clarity, choose the first available platform. */
	if(numPlatforms > 0)
	{
		cl_platform_id* platforms = (cl_platform_id* )malloc(numPlatforms* sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		if(status == CL_SUCCESS) printf("Selecting Plataform Success\n");
		else printf("Selecting Plataform failed\n");
		platform = platforms[0];
		free(platforms);
	}

	/*Step 2:Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device.*/
	cl_uint				numDevices = 0;
	cl_device_id        *devices;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);	
	if (numDevices == 0)	//no GPU available.
	{
		cout << "No GPU device available." << endl;
		cout << "Choose CPU as default device." << endl;
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);	
		if(status == CL_SUCCESS) printf("Selecting CPU Success\n");
		else printf("Selecting CPU failed\n");
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
		if(status == CL_SUCCESS) printf("Divices  Success\n");
		else printf("Devices failed\n");
	}
	else
	{
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	}
	

	/*Step 3: Create context.*/
	cl_context context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);
	if(status == CL_SUCCESS) printf("Context Success\n");
	else printf("context failed\n");
	/*Step 4: Creating command queue associate with the context.*/
	cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);
	if(status == CL_SUCCESS) printf("Command Queue Success\n");
	else printf("Command Queue failed\n");
	/*Step 5: Create program object */
	const char *filename = "mult.cl";
	string sourceStr;
	status = convertToString(filename, sourceStr);
	const char *source = sourceStr.c_str();
	size_t sourceSize[] = {strlen(source)};
	printf("PROGRAM:\n%s\n", sourceStr.c_str());
	cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, &status);
	if(status == CL_SUCCESS) printf("Creating Program Success\n");
	else printf("%d ERROR code, C program failed\n", status);
	/*Step 6: Build program. */
	printf("%d\n Num Devices\n", numDevices);
	status=clBuildProgram(program, numDevices ,devices,NULL,NULL,NULL);
	if(status == CL_SUCCESS)
	{
		printf("Building Program Successed!\n");
	}
	else printf("%d ERROR code, Build failed\n", status);
	int *input_a;
	int *input_b;
	int *output;

	input_a = rndarr(input_size);
	input_b = rndarr(input_size);
	prnarr(input_a, input_size);
	prnarr(input_b, input_size);
	output = (int*) malloc(input_size*sizeof(int));
	/*Step 7: Initial input,output for the host and create memory objects for the kernel*/
	
	/*
	const char* input = "GdkknVnqkc";
	size_t strlength = strlen(input);
	cout << "input string:" << endl;
	cout << input << endl;
	char *output = (char*) malloc(strlength + 1);
	*/

	cl_mem inputBufferA = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, input_size * sizeof(int),(void *) input_a, NULL);
	status = clEnqueueWriteBuffer(
						commandQueue,
						inputBufferA,
						CL_TRUE,
						0,
						input_size * sizeof(int),
						(void *)input_a,
						0,
						NULL,
						NULL);
	if(status == CL_SUCCESS) printf("EBA\n");
	cl_mem inputBufferB = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, input_size * sizeof(int),(void *) input_b, NULL);
	status = clEnqueueWriteBuffer(
						commandQueue,
						inputBufferB,
						CL_TRUE,
						0,
						input_size * sizeof(int),
						(void *)input_b,
						0,
						NULL,
						NULL);
	if(status == CL_SUCCESS) printf("EBA\n");
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY , input_size * sizeof(int), NULL, NULL);
	

	/*Step 8: Create kernel object */
	cl_kernel kernel = clCreateKernel(program,"mult", NULL);

	/*Step 9: Sets Kernel arguments.*/
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBufferA);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &inputBufferB);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &outputBuffer);
	
	/*Step 10: Running the kernel.*/
	size_t global_work_size[1] = {(size_t) input_size};
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

	/*Step 11: Read the cout put back to host memory.*/
	status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, input_size * sizeof(int), output, 0, NULL, NULL);
	
	/*
	output[strlength] = '\0';	//Add the terminal character to the end of output.
	cout << "\noutput string:" << endl;
	cout << output << endl;
	*/
	prnarr(output, input_size);

	/*Step 12: Clean the resources.*/
	status = clReleaseKernel(kernel);				//Release kernel.
	status = clReleaseProgram(program);				//Release the program object.
	status = clReleaseMemObject(inputBufferA);		//Release mem object.
	status = clReleaseMemObject(inputBufferB);
	status = clReleaseMemObject(outputBuffer);
	status = clReleaseCommandQueue(commandQueue);	//Release  Command queue.
	status = clReleaseContext(context);				//Release context.

	if (output != NULL)
	{
		free(output);
		output = NULL;
	}

	if (devices != NULL)
	{
		free(devices);
		devices = NULL;
	}

	std::cout<<"Passed!\n";
	return SUCCESS;
}
#pragma clang diagnostic pop