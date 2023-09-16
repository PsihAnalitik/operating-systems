#include <Windows.h>
#include <iostream>
#include <fstream>
#include <time.h>


#define BLOCK_SIZE 1024
#define DATA_SIZE (256*1024)
#define NUM_iTERATIONS 10

void fillRandomData(unsigned char* data, int size){
    for (int i=0; i <size; i++){
        data[i] = rand()%256;
    }
}

void writeToExternalMemory(const char* externalFileName, const unsigned char* data, int dataSize, const int blockSize){
    HANDLE fout;

    fout = CreateFileA(
                externalFileName,              
                GENERIC_WRITE,          
                0,       
                NULL,                  
                TRUNCATE_EXISTING,         
                FILE_ATTRIBUTE_NORMAL, 
                NULL
            );
    int numBlocks = dataSize / blockSize;
    int remainingBytes = dataSize % blockSize;

    for (int i =0; i < numBlocks; i++){
        DWORD bytess;
        WriteFile(fout, &data[i*blockSize], BLOCK_SIZE / blockSize, &bytess, NULL);
    }
    CloseHandle(fout);

}

int main(){
    using namespace std;
    LARGE_INTEGER start, finish, perSecond;
    unsigned char dataArray[DATA_SIZE];
    int* block_size = new int[5];
    for (int i =0; i<5; i++){
        if(i <1){
        block_size[i] = 512;
        }
        block_size[i] = 512 + i*128;

    }
    srand(time(NULL));
    fillRandomData(dataArray,DATA_SIZE);
    ofstream file;
    file.open("Analyze_time.txt");
    for (int i=0; i < NUM_iTERATIONS; i++){
        file << endl << "iter " << i << endl;
        cout << endl << "iter " << i << endl;
        for(int j=0; j < 5; j++){
            QueryPerformanceCounter(&start);
            writeToExternalMemory("External_Memory.txt", dataArray,DATA_SIZE, block_size[j]);
            QueryPerformanceCounter(&finish);
            QueryPerformanceFrequency(&perSecond);
            double timeDelta = (double)(finish.QuadPart - start.QuadPart) / perSecond.QuadPart;
            file << block_size[j] << " " << block_size[j] / timeDelta / 1024.0 << endl;
            cout << block_size[j] << " " << block_size[j] / timeDelta / 1024.0 << endl;
        }
    }
    cin.get();
    return 0;
}