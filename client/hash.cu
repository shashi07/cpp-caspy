#include <stdio.h>
#include <iostream>
#include "gpuhash.h"
#include "sha1.h"

#define BUFLEN (500*1024*1024)
#define PACKET_SIZE (4*1024)
#define BLOCK_SIZE 128

using namespace std;

__constant__ int thread_counts;

__device__ unsigned int roll(const unsigned int value,
                const unsigned int steps)
        {
            return ((value << steps) | (value >> (32 - steps)));
        }


__device__ void clearwBuffert(unsigned int* buffert)
        {
            for (int pos = 16; --pos >= 0;)
            {
                buffert[pos] = 0;
            }
        }

__device__ void innerhash(unsigned int* result, unsigned int* w)
        {
            unsigned int a = result[0];
            unsigned int b = result[1];
            unsigned int c = result[2];
            unsigned int d = result[3];
            unsigned int e = result[4];

            int round = 0;

            #define sha1macro(func,val) \
            { \
                const unsigned int t = roll(a, 5) + (func) + e + val + w[round]; \
                e = d; \
                d = c; \
                c = roll(b, 30); \
                b = a; \
                a = t; \
            }

            while (round < 16)
            {
                sha1macro((b & c) | (~b & d), 0x5a827999)
                ++round;
            }
            while (round < 20)
            {
                w[round] = roll((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
                sha1macro((b & c) | (~b & d), 0x5a827999)
                ++round;
            }
            while (round < 40)
            {
                w[round] = roll((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
                sha1macro(b ^ c ^ d, 0x6ed9eba1)
                ++round;
            }
            while (round < 60)
            {
                w[round] = roll((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
                sha1macro((b & c) | (b & d) | (c & d), 0x8f1bbcdc)
                ++round;
            }
            while (round < 80)
            {
                w[round] = roll((w[round - 3] ^ w[round - 8] ^ w[round - 14] ^ w[round - 16]), 1);
                sha1macro(b ^ c ^ d, 0xca62c1d6)
                ++round;
            }

            #undef sha1macro

            result[0] += a;
            result[1] += b;
            result[2] += c;
            result[3] += d;
            result[4] += e;
        }





__device__ void calculate(const void* src, const int bytelength, unsigned char* hash)
    {
        // Init the result array.
        unsigned int result[5] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };

        // Cast the void src pointer to be the byte array we can work with.
        const unsigned char* sarray = (const unsigned char*) src;

        // The reusable round buffer
        unsigned int w[80];

        // Loop through all complete 64byte blocks.
        const int endOfFullBlocks = bytelength - 64;
        int endCurrentBlock;
        int currentBlock = 0;

        while (currentBlock <= endOfFullBlocks)
        {
            endCurrentBlock = currentBlock + 64;

            // Init the round buffer with the 64 byte block data.
            for (int roundPos = 0; currentBlock < endCurrentBlock; currentBlock += 4)
            {
                // This line will swap endian on big endian and keep endian on little endian.
                w[roundPos++] = (unsigned int) sarray[currentBlock + 3]
                        | (((unsigned int) sarray[currentBlock + 2]) << 8)
                        | (((unsigned int) sarray[currentBlock + 1]) << 16)
                        | (((unsigned int) sarray[currentBlock]) << 24);
            }
            innerhash(result, w);
        }

        // Handle the last and not full 64 byte block if existing.
        endCurrentBlock = bytelength - currentBlock;
        clearwBuffert(w);
        int lastBlockBytes = 0;
        for (;lastBlockBytes < endCurrentBlock; ++lastBlockBytes)
        {
            w[lastBlockBytes >> 2] |= (unsigned int) sarray[lastBlockBytes + currentBlock] << ((3 - (lastBlockBytes & 3)) << 3);
        }
        w[lastBlockBytes >> 2] |= 0x80 << ((3 - (lastBlockBytes & 3)) << 3);
        if (endCurrentBlock >= 56)
        {
            innerhash(result, w);
            clearwBuffert(w);
        }
        w[15] = bytelength << 3;
        innerhash(result, w);

        for (int hashByte = 20; --hashByte >= 0;)
        {
            hash[hashByte] = (result[hashByte >> 2] >> (((3 - hashByte) & 0x3) << 3)) & 0xff;
        }
    }


__global__
void calculateHash(unsigned char *gdata, unsigned char *hash)
{

    int i=blockIdx.x * blockDim.x + threadIdx.x;
    if(i<thread_counts){
    calculate(&gdata[i*PACKET_SIZE],PACKET_SIZE,&hash[i*20]);
    }
}



int calc_gpu(unsigned char *data1, int charRead, thread_data_t * &t)
{

        unsigned char *gdata;
        unsigned char *hash;
        const int gdsize = charRead*sizeof(char);
        
        cudaMalloc((void**)&gdata,gdsize);
        cudaError_t cuda_result_code = cudaGetLastError();
        
        if (cuda_result_code!=cudaSuccess) {
           printf("gdata message: %s\n",cudaGetErrorString(cuda_result_code));
        }

        int threads = charRead/(PACKET_SIZE);
        
	    int blocks = threads/BLOCK_SIZE;
	    if(threads%BLOCK_SIZE) 
		      blocks++;
        
        const int hash_size = 20 * blocks * BLOCK_SIZE * sizeof(unsigned char);
        
        cudaMalloc((void**)&hash, hash_size);

        cuda_result_code = cudaGetLastError();

        if (cuda_result_code!=cudaSuccess) {
                  printf("hash message: %s\n",cudaGetErrorString(cuda_result_code));
               }

        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        // Start timer
        cudaEventRecord(start);

        cudaMemcpy( gdata, data1, gdsize, cudaMemcpyHostToDevice );

        if (cuda_result_code!=cudaSuccess) {
                   printf(" memcpy message: %s\n",cudaGetErrorString(cuda_result_code));
                }
                
        cudaMemcpyToSymbol(thread_counts, &threads, sizeof(int));

        calculateHash<<<blocks, BLOCK_SIZE>>>(gdata,hash);
        
        cudaDeviceSynchronize();
        
        if (cuda_result_code!=cudaSuccess) {
                   printf(" hashcalucation message: %s\n",cudaGetErrorString(cuda_result_code));
                }

        unsigned char * hash_host = new unsigned char[hash_size];
        cudaMemcpy( hash_host, hash, hash_size, cudaMemcpyDeviceToHost);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);
        float milliseconds = 0;
        cudaEventElapsedTime(&milliseconds, start, stop);

        cuda_result_code = cudaGetLastError();

        if (cuda_result_code!=cudaSuccess) {
                  printf("hash message: %s\n",cudaGetErrorString(cuda_result_code));
               }

        cout <<"\nTime taken : "<<milliseconds<<"ms"<<endl;
        cudaFree( gdata );
        cudaFree( hash );

        cout<< charRead;
        int remainders = charRead % PACKET_SIZE;
        if (remainders)
            threads++;
        t = new thread_data_t[threads];
        int len=PACKET_SIZE;
        for(int i=0;i<threads;i++)
        {
            if(i==threads-1){
                len = remainders;
                sha1::calc(&data1[i*PACKET_SIZE],len,&hash_host[i*20]);
                cout<<"len :"<<len<<endl;
            }
            t[i].data = new char[len];
            memcpy ( t[i].data, &data1[i*PACKET_SIZE], len);
            t[i].len = len;
            memcpy ( t[i].hash, &hash_host[i*20], 20);
            /*
            cout<<"Hash :";
        	for(int j=0;j<20;j++)
        		printf("%02x",hash_host[i*20+j]);
            cout<<endl; */
        }

        delete[] hash_host;
    printf("Completed");
    return threads;
}
