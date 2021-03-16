#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <cstring>

#include "../encodage.cpp"
#include <chrono> 
using namespace std::chrono;

#define TAILLE_MOT               7  
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)
#define TAILLE_MOT        7

char alphabet[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int myIndex[TAILLE_MOT] = {0,0,0,0,0,0,0};

int main(void)
{
        auto start = high_resolution_clock::now(); 
        void incrementIndex(int rangIndex);
        string mot_a_trouver = "aanjour";
        string str_hash_a_trouver = encode(mot_a_trouver);
        char hash_a_trouver[TAILLE_MOT] ;
        char hash_test[TAILLE_MOT];
        strcpy(hash_a_trouver, str_hash_a_trouver.c_str());

        for (int i=0; i< TAILLE_MOT; i++)
                cout << hex << (int)hash_a_trouver[i] << " ";
        cout << "\n" << endl;


        // Load the kernel source code into the array source_str
        FILE *fp;
        char *source_str;
        size_t source_size;

        fp = fopen("decoder_kernel.cl", "r");
        if (!fp) {
                fprintf(stderr, "Failed to load kernel.\n");
                exit(1);
        }
        source_str = (char*)malloc(MAX_SOURCE_SIZE);
        source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
        fclose( fp );

        // Get platform and device information
        cl_platform_id platform_id = NULL;
        cl_device_id device_id = NULL;   
        cl_uint ret_num_devices;
        cl_uint ret_num_platforms;
        cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
        ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_ALL, 1, &device_id, &ret_num_devices);

        // Create an OpenCL context
        cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

        // Create a command queue
        cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);


        //memory buffer for each var
        cl_mem hash_a_trouver_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, TAILLE_MOT * sizeof(char), NULL, &ret);
        cl_mem hash_a_tester_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, TAILLE_MOT * sizeof(char), NULL, &ret);
        cl_mem alphabet_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 26 * sizeof(char), NULL, &ret);

        // Copy variables to their respective memory buffers
        ret = clEnqueueWriteBuffer(command_queue, hash_a_trouver_mem_obj, CL_TRUE, 0, TAILLE_MOT * sizeof(char), hash_a_trouver, 0, NULL, NULL);
        ret = clEnqueueWriteBuffer(command_queue, hash_a_tester_mem_obj, CL_TRUE, 0, TAILLE_MOT * sizeof(char), hash_test, 0, NULL, NULL);
        ret = clEnqueueWriteBuffer(command_queue, alphabet_mem_obj, CL_TRUE, 0, 26 * sizeof(char), alphabet, 0, NULL, NULL);

        // Create a program from the kernel source
        cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

        // Build the program
        ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

        // Create the OpenCL kernel
        cl_kernel kernel = clCreateKernel(program, "decoder", &ret);

        // Set the arguments of the kernel
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&hash_a_trouver_mem_obj);
        ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&hash_a_tester_mem_obj);
        ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&alphabet_mem_obj);
        
        // Execute the OpenCL kernel on the list
        size_t global_item_size = 1; // Process the entire lists
        size_t local_item_size = 1; // Process in groups of 64
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

        // Read the memory buffer C on the device to the local variable C
        //int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
        ret = clEnqueueReadBuffer(command_queue, hash_a_tester_mem_obj, CL_TRUE, 0, TAILLE_MOT * sizeof(char), hash_test, 0, NULL, NULL);
         

        // Display the result to the screen
        //for(i = 0; i < LIST_SIZE; i++)
        //   printf("%d + %d = %d\n", A[i], B[i], C[i]);

        // Clean up
        ret = clFlush(command_queue);
        ret = clFinish(command_queue);
        ret = clReleaseKernel(kernel);
        ret = clReleaseProgram(program);

        ret = clReleaseMemObject(hash_a_trouver_mem_obj);
        ret = clReleaseMemObject(hash_a_tester_mem_obj);
        ret = clReleaseMemObject(alphabet_mem_obj);

        ret = clReleaseCommandQueue(command_queue);
        ret = clReleaseContext(context);

       // free(A);
       // free(B);
        //free(C);

        auto stop = high_resolution_clock::now(); 
        cout << "---------------------------------------- \n";
        cout << "mot trouvé : "<< hash_test << "\n";
        cout << "hash trouvé : \n";
        for (size_t i=0; i< TAILLE_MOT; i++)
                cout << hex << (int)hash_test[i] << " ";
        cout << "\n" << endl;
        auto duration = duration_cast<seconds>(stop - start); 

        cout << "Time taken by function: " << duration.count() << " seconds" << endl;

        return 0;
}

