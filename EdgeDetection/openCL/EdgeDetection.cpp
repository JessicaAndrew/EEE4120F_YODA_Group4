// based of code by Christopher Hill

// include needed libraries
#include<stdio.h>
#include<CL/cl.h>
#include<iostream>
#include<fstream>
#include<string>
#include<cmath>
#include <tuple>
#include<math.h>

using namespace std;

// function to read in picture information (row, column, pixel count) from the input file and store (using a passed in pointer reference) in an array
void getPicInfo(int* picInfo){
	FILE *inFile  = fopen("picInfo.txt", "r"); //read in file
	int temp;
	int i = 0; // counter value

	// code to scan in the file line by line
	while (fscanf(inFile, "%d", & temp ) == 1){ 
		picInfo[i] = temp;
		i = i + 1;
	} 
}

// function to read in pixel values from the input file and store (using a passed in pointer reference) in an array
void create1DImageMatrix(int size, int* imgMatrix){
	FILE *inFile  = fopen("In.txt", "r"); //read in file
	int temp;
	int temp2 = 0; // value to allow for debugging where the size of the output pixel array needs to be limited
	int i = 0; // counter value

	// code to scan in the file line by line
	while (fscanf(inFile, "%d", & temp ) == 1 && temp2<=size){ 
		imgMatrix[i] = temp;
		i = i + 1;
		temp2 = temp2 +1;
	} 
}

int main(void){
	clock_t startPrint1, endPrint1, startProcess, endProcess, startOverhead1, endOverhead1, startOverhead2, endOverhead2;  //Timers
	startOverhead1 = clock(); //start the first clock for timing the overhead

	//matrices used to compute the respective X and Y gradients of the input image
	int kernelSize = 9;
	int xKern[kernelSize] = {-1,0,1,-2,0,2,-1,0,1};
	int yKern[kernelSize] = {-1,-2,-1,0,0,0,1,2,1};

	int picInfo[3]; //array to store row, column and pixel counts
	getPicInfo(picInfo); //get picture information
	int rowSize = picInfo[0]; //total numbr of rows in the image
	int colSize = picInfo[1]; //total numbr of columns in the image
	int countPixels = picInfo[2]; //total numbr of pixels in the image

	int imgMatrix[countPixels]; //array to store pixel values
	create1DImageMatrix(countPixels,imgMatrix); //get picture's pixels

	//------------------------------------------------------------------------
	 
	//Initialize Buffers, memory space that allows for communication between the host and the target device
	//Initialize imgMatrix_buffer, output_buffer, rowSize_buffer, colSize_buffer, countPixels_buffer, and filterSize_buffer
	cl_mem imgMatrix_buffer, output_buffer, rowSize_buffer, colSize_buffer, countPixels_buffer, xKern_buffer, yKern_buffer; //replace filter buffer with kern

	//Get the platform you want to use
	//cl_int clGetPlatformIDs(cl_uint num_entries, cl_platform_id *platforms, cl_uint *num_platforms)
  	cl_uint platformCount; //keeps track of the number of platforms you have installed on your device
	cl_platform_id *platforms;
	clGetPlatformIDs(5, NULL, &platformCount); //sets platformCount to the number of platforms
	platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * platformCount); //get all platforms
	clGetPlatformIDs(platformCount, platforms, NULL); //saves a list of platforms in the platforms variable
	cl_platform_id platform = platforms[0]; //Select the platform you would like to use in this program (change index to do this). If you would like to see all available platforms run platform.cpp.
	
	//Outputs the information of the chosen platform
	char* Info = (char*)malloc(0x1000*sizeof(char));
	clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0x1000, Info, 0);
	//printf("Name      : %s\n", Info);
	clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, 0x1000, Info, 0);
	//printf("Vendor    : %s\n", Info);
	clGetPlatformInfo(platform, CL_PLATFORM_VERSION, 0x1000, Info, 0);
	//printf("Version   : %s\n", Info);
	clGetPlatformInfo(platform, CL_PLATFORM_PROFILE, 0x1000, Info, 0);
	//printf("Profile   : %s\n", Info);
	
	//------------------------------------------------------------------------

	//Get device ID must first get platform
	//cl_int clGetDeviceIDs(cl_platform_id platform, cl_device_type device_type, cl_uint num_entries, cl_device_id *devices, cl_uint *num_devices)
	cl_device_id device; //this is your deviceID
	cl_int err;
	
	//Access a device
	//The if statement checks to see if the chosen platform uses a GPU, if not it setups the device using the CPU
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	if(err == CL_DEVICE_NOT_FOUND) {
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
	}
	//printf("Device ID = %i\n",err);

	//------------------------------------------------------------------------
	
	//Creates a context that allows devices to send and receive kernels and transfer data
	//cl_context clCreateContext(cl_context_properties *properties, cl_uint num_devices, const cl_device_id *devices, void *pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data), void *user_data,cl_int *errcode_ret)
	cl_context context; //This is your contextID, the line below must just run
	context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);

	//------------------------------------------------------------------------

	//Get details about the kernel.cl file in order to create it (read the kernel.cl file and place it in a buffer)
	FILE *program_handle; //read file in	
	program_handle = fopen("OpenCL/Kernel.cl", "r");

	//Get program size
	size_t program_size;//, log_size;
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);

	//Sort buffer out
	char *program_buffer;//, *program_log;
	program_buffer = (char*)malloc(program_size + 1);
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);
   	
	//------------------------------------------------------------------------

	//Create program from source because the kernel is in a separate file 'kernel.cl', therefore the compiler must run twice once on main and once on kernel
	//cl_program clCreateProgramWithSource (cl_context context, cl_uint count, const char **strings, const size_t *lengths, cl_int *errcode_ret)	
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&program_buffer, &program_size, NULL); //this compiles the kernels code

	//------------------------------------------------------------------------

	//Build the program, this compiles the source code from above for the devices that the code has to run on (ie GPU or CPU)
	//cl_int clBuildProgram(cl_program program, cl_uint num_devices, const cl_device_id* device_list, const char* options, void (CL_CALLBACK* pfn_notify)(cl_program program, void* user_data), void* user_data);
	cl_int err3= clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	//printf("program ID = %i\n", err3);
	
	//------------------------------------------------------------------------

	//Creates the kernel, this creates a kernel from one of the functions in the cl_program you just built
	//cl_kernel clCreateKernel(cl_program program, const char* kernel_name, cl_int* errcode_ret);
	cl_kernel kernel = clCreateKernel(program, "edgeDetection", &err); //select the kernel you are running

	//------------------------------------------------------------------------
	
	//Create command queue to the target device. This is the queue that the kernels get dispatched too, to get the the desired device.
	//cl_command_queue clCreateCommandQueue(cl_context context, cl_device_id device, cl_command_queue_properties properties, cl_int *errcode_ret)
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, 0, NULL);

	//------------------------------------------------------------------------

	//Create data buffers for memory management between the host and the target device
	//set global_size, local_size and num_groups, in order to control the number of work item in each work group
	size_t global_size = colSize*rowSize; // total number of work items is total number of items (pixels) in the input matrix
	size_t local_size = colSize; //size of each work group is the number of columns in the input matrix
	cl_int num_groups = global_size/local_size; //number of work groups needed is the number of rows in the input matrix
	//int xGrad[global_size];
	//int yGrad[global_size];


	//Initialize the output array
	int output[global_size]; //output matrix

	//Buffer (memory block) that both the host and target device can access 
	//cl_mem clCreateBuffer(cl_context context, cl_mem_flags flags, size_t size, void* host_ptr, cl_int* errcode_ret);
	//create imgMatrix_buffer, output_buffer, rowSize_buffer, colSize_buffer, countPixels_buffer, and filterSize_buffer
	imgMatrix_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, global_size*sizeof(int), &imgMatrix, &err);
	output_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, global_size*sizeof(int), output, &err);
	rowSize_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &rowSize, &err);
	colSize_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &colSize, &err);
	// could this be an issue?? see below
	countPixels_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &countPixels, &err);
	xKern_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, kernelSize*sizeof(int), &xKern, &err);
	yKern_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, kernelSize*sizeof(int), &yKern, &err);
	// JESS filterSize_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &filterSize, &err);
	
	//------------------------------------------------------------------------

	//Create the arguments for the kernel (link these to the buffers set above, using the pointers for the respective buffers)
	//cl_int clSetKernelArg (cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void *arg_value)
	//Create the arguments for the kernel. Note you can create a local buffer only on the GPU as follows: clSetKernelArg(kernel, argNum, size, NULL);
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &imgMatrix_buffer);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buffer);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &rowSize_buffer);
	clSetKernelArg(kernel, 3, sizeof(cl_mem), &colSize_buffer);
	clSetKernelArg(kernel, 4, sizeof(cl_mem), &countPixels_buffer);
	clSetKernelArg(kernel, 5, sizeof(cl_mem), &xKern_buffer);
	clSetKernelArg(kernel, 6, sizeof(cl_mem), &yKern_buffer);
	
	endOverhead1 = clock(); //end the first clock for timing the overhead
    startProcess = clock(); //start the clock for timing the processes

	//------------------------------------------------------------------------

	//Enqueue kernel, deploys the kernels and determines the number of work-items that should be generated to execute the kernel (global_size) and the number of work-items in each work-group (local_size).
	//cl_int clEnqueueNDRangeKernel (cl_command_queue command_queue, cl_kernel kernel, cl_uint work_dim, const size_t *global_work_offset, const size_t *global_work_size, const size_t *local_work_size, cl_uint num_events_in_wait_list, const cl_event *event_wait_list, cl_event *event)
	cl_int err4 = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL); 
	//printf("\nKernel check: %i \n",err4);

	//------------------------------------------------------------------------

	//Allows the host to read from the buffer object 
	err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, sizeof(output), output, 0, NULL, NULL);
	
	//This command stops the program here until everything in the queue has been run
	clFinish(queue);

	endProcess = clock(); //end the clock for timing the processes
    startPrint1 = clock(); //start the first clock for timing the print

	//sending output pixel values to a text file
	FILE *filePrint = fopen("Out.txt","w");
	for(int n = 0; n<(countPixels); n++){
		// if statement used to not put a new line for the last line
   		if (n != (countPixels - 1)){
			fprintf(filePrint,"%d\n", output[n]);
			
		}else{
			fprintf(filePrint,"%d", output[n]);
		}
	}
	fclose(filePrint);
	
	/*printf("\nInput in the input_buffer \n");
	for(int j=0; j<countPixels; j++) {
		printf("%i \t " ,imgMatrix[j]);
		if(j%colSize == (colSize-1)){
			printf("\n");
		}
	}
	printf("\nOutput in the output_buffer \n");
	for(int j=0; j<countPixels; j++) {
		printf("%i \t " ,output[j]);
		if(j%colSize == (colSize-1)){
			printf("\n");
		}
	}*/

	endPrint1 = clock(); //end the first clock for timing the print
	startOverhead2 = clock(); //start the second clock for timing the overhead
	
	//------------------------------------------------------------------------

	//Deallocate resources	
	clReleaseKernel(kernel);
	clReleaseMemObject(imgMatrix_buffer);
	clReleaseMemObject(output_buffer);
	clReleaseMemObject(rowSize_buffer);
	clReleaseMemObject(colSize_buffer);
	clReleaseMemObject(countPixels_buffer);
	clReleaseMemObject(xKern_buffer);
	clReleaseMemObject(yKern_buffer);

	//JESS clReleaseMemObject(filterSize_buffer);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);

	endOverhead2 = clock(); //end the second clock for timing the overhead

	//calculate and print out the time for the overhead and processes
	printf ("Overhead Time: %0.8f sec \n", ((float) endOverhead2 + endOverhead1 - startOverhead1 - startOverhead2)/CLOCKS_PER_SEC);
	printf ("Run Time of edge detection (OpenCL): %0.8f sec \n", ((float) endProcess - startProcess)/CLOCKS_PER_SEC);
	printf ("Save to file Run Time of median filter (golden measure): %0.8f sec \n", ((float) endPrint1 - startPrint1)/CLOCKS_PER_SEC); //print the timing of the golden standard
	FILE *filePrintInfo = fopen("OutEdgeOpenCLSpeed.txt","w");
	fprintf(filePrintInfo,"%0.8f\n", (((float) endProcess - startProcess)/CLOCKS_PER_SEC));
	fprintf(filePrintInfo,"%0.8f\n", ((float) endOverhead2 + endOverhead1 - startOverhead1 - startOverhead2)/CLOCKS_PER_SEC);
	fclose(filePrintInfo);

	return 0;
}