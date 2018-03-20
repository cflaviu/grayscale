#include "PCH.h"
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
	return -1;
}

const char* const bmpFiles[] =
{
	"bird",
	"lena",
	"test_32bpp_8888.bmp",
	"test_24bpp_888.bmp",
	"test_16bpp_565.bmp",
	"test_16bpp_1555.bmp"
};

unsigned int roundUp(unsigned int value, unsigned int multiple) 
{
	unsigned int remainder = value % multiple;
	if ( remainder) 
	{
		value += ( multiple - remainder);
	}

	return value;
}

int run()
{
	/*Step1: Getting platforms and choose an available one.*/
	cl_uint numPlatforms;//the NO. of platforms
	cl_platform_id platform = NULL;//the chosen platform
	cl_int	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		cout<<"Error: Getting platforms!"<<endl;
		return 1;
	}

	/*For clarity, choose the first available platform. */
	if(numPlatforms > 0)
	{
		cl_platform_id* platforms = (cl_platform_id* )malloc(numPlatforms* sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}

	/*Step 2:Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device.*/
	cl_uint				numDevices = 0;
	cl_device_id        *devices;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);	
	if (numDevices == 0) //no GPU available.
	{
		//cout << "No GPU device available."<<endl;
		//cout << "Choose CPU as default device."<<endl;
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);	
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
	}
	else
	{
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	}
	

	/*Step 3: Create context.*/
	cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);
	
	/*Step 4: Creating command queue associate with the context.*/
	cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);

	/*Step 7: Initial input,output for the host and create memory objects for the kernel*/
	int fileNo = 0;
	string fileName;// = "..\\";
	fileName += bmpFiles[ fileNo];
	fileName += ".bmp";

	const int hs = 100;
	char header[ hs];

	BmpInfo bmpInfo;
	FILE* file = fopen( fileName.c_str(), "rb");
	assert( file);
	fread( header, 1, hs, file);
	fseek( file, 0, SEEK_SET);
	if ( !file || !bmpInfo.open( file))
	{
		int x = 0;
	}

	int width = bmpInfo.width();
	int height = bmpInfo.height();
	int imageSize = bmpInfo.imageSize();
	byte* buffer1 = new byte[ imageSize];
	if ( !readBitmap( (void*) buffer1, file, bmpInfo.offset(), imageSize))
	{
		int x = 0;
	}

	cl_image_format imageFormat1;
	imageFormat1.image_channel_order = CL_RGBA;
	imageFormat1.image_channel_data_type = CL_UNSIGNED_INT8;

	cl_image_format imageFormat2;
	imageFormat2.image_channel_order = CL_RGBA;
	imageFormat2.image_channel_data_type = CL_UNSIGNED_INT8;

    cl_image_desc imageDesc1;
    memset(&imageDesc1, 0, sizeof(cl_image_desc));
    imageDesc1.image_type = CL_MEM_OBJECT_IMAGE2D;
    imageDesc1.image_width = width;
    imageDesc1.image_height = height;

	//const float coff[] = { 0.33f, 0.59f, 0.11f};
	//const float coff[] = { 1, 0, 0,  0, 1, 0,  0, 0, 1};

	float coff[ 9];
	coff[0]= 1.05f;
	coff[1]= -0.025f;
	coff[2]= -0.025f;
	coff[3]= -0.45f;
	coff[4]= 1.975f;
	coff[5]= -0.525f;
	coff[6]= 0.225f;
	coff[7]= -1.05f;
	coff[8]= 1.825f;

	cl_mem inputcoff	= clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(coff), (void*) coff, &status);
	cl_mem inputImage	= clCreateImage(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, &imageFormat1, &imageDesc1, buffer1, &status);
	cl_mem outputImage	= clCreateImage(context, CL_MEM_WRITE_ONLY, &imageFormat2, &imageDesc1, 0, &status);

	/*Step 5: Create program object */
	const char *filename = "Grayscale.cl";
	string sourceStr;
	status = convertToString(filename, sourceStr);
	assert(!status);
	const char *source = sourceStr.c_str();
	size_t sourceSize[] = {strlen(source)};
	cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
	
	status = clBuildProgram(program, 1,devices,NULL,NULL,NULL);
	assert(!status);

	status = clEnqueueWriteBuffer( commandQueue, inputcoff, CL_FALSE, 0, sizeof(coff), (void*)coff, 0, NULL, NULL);
	assert(!status);

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { width, height, 1};
	status = clEnqueueWriteImage(commandQueue, inputImage, CL_FALSE, origin, region, 0, 0, buffer1, 0, NULL, NULL);
	assert(!status);

	cl_kernel kernel = clCreateKernel(program, "grayscale", NULL);

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputcoff);
	assert(!status);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&inputImage);
	assert(!status);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&outputImage);
	assert(!status);
	
	/*Step 10: Running the kernel.*/

	int paddingPixels = 0;
	// Selected workgroup size is 16x16
	int wgWidth = 16;
	int wgHeight = 16;
	// When computing the total number of work-items, the
	// padding work-items do not need to be considered
	int totalWorkItemsX = roundUp(width-paddingPixels, wgWidth);
	int totalWorkItemsY = roundUp(height-paddingPixels, wgHeight);

	// Size of a workgroup
	size_t localSize[2] = {wgWidth, wgHeight};
	// Size of the NDRange
	size_t globalSize[2] = {totalWorkItemsX, totalWorkItemsY};

	status = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);
	assert(!status);
	
	status = clFinish(commandQueue);
	assert(!status);

	status = clEnqueueReadImage(commandQueue, outputImage, CL_TRUE, origin, region, 0, 0, buffer1, 0, NULL, NULL);
	assert(!status);

	string outFile = bmpFiles[ 0];
	outFile += "_out.bmp";
	FILE *nk = fopen( outFile.c_str(), "wb");
	fwrite( header, 1, hs, nk);
	fseek( nk, bmpInfo.offset(), SEEK_SET);
	fwrite( buffer1, 1, imageSize, nk);
	fclose(nk);
	
	delete [] buffer1;

	/*Step 12: Clean the resources.*/
	status = clReleaseKernel(kernel);//*Release kernel.
	assert(!status);
	status = clReleaseProgram(program);	//Release the program object.
	assert(!status);
	status = clReleaseMemObject(inputcoff);//Release mem object.
	assert(!status);
	status = clReleaseMemObject(inputImage);//Release mem object.
	assert(!status);
	status = clReleaseMemObject(outputImage);
	assert(!status);
	status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
	assert(!status);
	status = clReleaseContext(context);//Release context.
	assert(!status);

	if (devices != NULL)
	{
		free(devices);
		devices = NULL;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	try
	{
		run();
	}
	catch( std::exception& ex)
	{
		const char* c = ex.what();
		c = 0;
	}
	catch(...)
	{
		int x = 0;
	}
	return 0;
}
