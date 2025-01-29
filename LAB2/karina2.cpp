#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/wait.h>

 
using namespace std;
 


typedef unsigned char BYTEread;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
 
struct tagBITMAPFILEHEADER{
WORD bfType; //specifies the file type
DWORD bfSize; //specifies the size in bytes of the bitmap file
WORD bfReserved1; //reserved; must be 0
WORD bfReserved2; //reserved; must be 0
DWORD bfOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
};
 
struct tagBITMAPINFOHEADER{
DWORD biSize; //specifies the number of bytes required by the struct
LONG biWidth; //specifies width in pixels
LONG biHeight; //species height in pixels
WORD biPlanes; //specifies the number of color planes, must be 1
WORD biBitCount; //specifies the number of bit per pixel
DWORD biCompression;//spcifies the type of compression
DWORD biSizeImage; //size of image in bytes
LONG biXPelsPerMeter; //number of pixels per meter in x axis
LONG biYPelsPerMeter; //number of pixels per meter in y axis
DWORD biClrUsed; //number of colors used by th ebitmap
DWORD biClrImportant; //number of colors that are important
};

// BYTEread Color_grading_update(BYTEread old_color, float new_grading)
// {
//         float new_color_fp = ((float)old_color/255) * new_grading;
//         int new_color = static_cast<int> (round(new_color_fp*255));

// if (new_color>255){ //caps within 255
//   new_color=255;
//         }else if (new_color<0){
//   new_color=0;
//         }

//    BYTEread new_color_byteread = (BYTEread)(new_color);
//         return new_color_byteread;
// }

// void pixelUpdate(BYTEread *data, int startAddr, int endAddr, int biWidth, long wantedWidth, float * bgr_grading)
// {
// BYTEread old_color;

// for (int y = startAddr; y <= endAddr; y++) {
//     for (int x = 0; x < biWidth; x++) {
// for (int z=0; z<2; z++){ // b g r in order
//                old_color = data[3*x+y*wantedWidth+z];
//   data[3*x+y*wantedWidth+z] = Color_grading_update(old_color, bgr_grading[z]);
// }
// }
// }
// }
 

int main(int argc, char*argv[]){ //argv parse command line arguments, argc = # of arguments
    clock_t time_req; //starts to measure the time
    time_req = clock();

// if (argc < 5) {
// cout << "Error: Insufficient arguments provided." << endl;
// cout << "Usage: ./program <imageFile> <outputFile> <operation> <contrastFactor>" << endl;
// return 1;
// }


//take in the arguments - input , [program name, imagefile, outputfile, operation, contrast factor]

string imageFile = argv[1]; //input
// string imageFile = "./flowers.bmp";
    string r_grading_s = argv[4]; //red
//    string r_grading_s = "1.0";
    string g_grading_s = argv[3]; //green
//    string g_grading_s = "0.5";
    string b_grading_s = argv[2]; //blue
//    string b_grading_s = "0.8";

float r_grading = std::stof(r_grading_s); //convert to float
float g_grading = std::stof(g_grading_s);
float b_grading = std::stof(b_grading_s);

float bgr_grading [3] = {b_grading, g_grading, r_grading};

  string outputFile = argv[5]; //output
// string outputFile = "output.bmp";


tagBITMAPFILEHEADER bitMapFileHeader; //call the struct
tagBITMAPINFOHEADER bitMapInfoHeader; //call the struct

BYTEread* data; // read the orginal data
BYTEread* data1; //read the copy of the data


FILE *fileIn = fopen(imageFile.c_str(), "rb");
if (fileIn == NULL) {
    cout << "File doesn't exist: " << imageFile << endl;
    return 1; // Exit the program if the file doesn't exist
}


//read the struct data from bitMapFileHeader
fread(&bitMapFileHeader.bfType, sizeof(bitMapFileHeader.bfType), 1, fileIn);
fread(&bitMapFileHeader.bfSize, sizeof(bitMapFileHeader.bfSize), 1, fileIn);
fread(&bitMapFileHeader.bfReserved1, sizeof(bitMapFileHeader.bfReserved1), 1, fileIn);
fread(&bitMapFileHeader.bfReserved2, sizeof(bitMapFileHeader.bfReserved2), 1, fileIn);
fread(&bitMapFileHeader.bfOffBits, sizeof(bitMapFileHeader.bfOffBits), 1, fileIn);
fread(&bitMapInfoHeader, sizeof(tagBITMAPINFOHEADER), 1, fileIn);

//must allocate enough space
   // int actual_pixel_seg_size = bitMapFileHeader.bfSize - 14 - sizeof(tagBITMAPINFOHEADER);

//data=(BYTEread*)malloc(bitMapFileHeader.bfSize);
    data = (BYTEread*) mmap(0, bitMapFileHeader.bfSize, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
data1 = (BYTEread*) mmap(0,bitMapFileHeader.bfSize, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
fread(data,bitMapFileHeader.bfSize , 1, fileIn);
for (int i = 0; i < bitMapFileHeader.bfSize; i++) {
        data1[i]=data[i]; //copies all the data incase of loss
    }
fclose(fileIn); // then close the file


LONG wantedWidth=3*bitMapInfoHeader.biWidth+(((bitMapInfoHeader.biWidth)*3)%4); //dealing w multiple of 4
 
//choose the opertion will each iterte throught
 

    // calculate mid point
int mid_point = bitMapInfoHeader.biHeight >> 1;


    // start fork process

//int pid = fork();

//clock_t start = clock();

  if (fork() == 0) // child process
  {
time_req = clock();
    //pixelUpdate(data, 0, (mid_point-1), bitMapInfoHeader.biWidth, wantedWidth, bgr_grading);
for (int y = 0; y< bitMapInfoHeader.biHeight/2; y++) {
        for (int x = 0; x < bitMapInfoHeader.biWidth; x++) {
           
            int test=3*x+y*wantedWidth;
            BYTEread b=data[3*x+y*wantedWidth]; //bgr format, given iterates through
            BYTEread g=data[3*x+y*wantedWidth+1];
            BYTEread r=data[3*x+y*wantedWidth+2];

            float bf=(float)b/255;
            float gf=(float)g/255;
            float rf=(float)r/255;
            //could add check
            float cgr= atof(r_grading_s.c_str());
            float cgg= atof(g_grading_s.c_str());
            float cgb= atof(b_grading_s.c_str());
                //multiplying normalized value by correction factor from terminal
            bf*=cgb;
            gf*=cgg;
            rf*=cgr;

            int bi=((int)(bf*255));
            if(bi>255){
                bi=255;
            }else if(bi<0){
                bi=0;
            }
            b=(BYTEread)(bi);

            int gi=((int)(gf*255));
            if(gi>255){
                gi=255;
            }else if(gi<0){
                gi=0;
            }
            g=(BYTEread)(gi);

            int ri=((int)(rf*255));
            if(ri>255){
                ri=255;
            }else if(ri<0){
                ri=0;
            }
            r=(BYTEread)(ri);
            data[3*x+y*wantedWidth]=b;
            data[3*x+y*wantedWidth+1]=g;
            data[3*x+y*wantedWidth+2]=r;
            //at this point the changed data is written to
        }
    }      
    return 0;
  }
  else
  {  // main process

      //pixelUpdate(data, mid_point, (bitMapInfoHeader.biHeight-1), bitMapInfoHeader.biWidth, wantedWidth, bgr_grading);
    for (int y = bitMapInfoHeader.biHeight/2; y < bitMapInfoHeader.biHeight; y++) {
        for (int x = 0; x < bitMapInfoHeader.biWidth; x++) {
           
            int test=3*x+y*wantedWidth;
            BYTEread b=data[3*x+y*wantedWidth]; //bgr format
            BYTEread g=data[3*x+y*wantedWidth+1];
            BYTEread r=data[3*x+y*wantedWidth+2];

            float bf=(float)b/255;
            float gf=(float)g/255;
            float rf=(float)r/255;
            //could add check
            float cgr= atof(r_grading_s.c_str());
            float cgg= atof(g_grading_s.c_str());
            float cgb= atof(b_grading_s.c_str());
                //multiplying normalized value by correction factor from terminal
            bf*=cgb;
            gf*=cgg;
            rf*=cgr;

            int bi=((int)(bf*255));
            if(bi>255){
                bi=255;
            }else if(bi<0){
                bi=0;
            }
            b=(BYTEread)(bi);

            int gi=((int)(gf*255));
            if(gi>255){
                gi=255;
            }else if(gi<0){
                gi=0;
            }
            g=(BYTEread)(gi);

            int ri=((int)(rf*255));
            if(ri>255){
                ri=255;
            }else if(ri<0){
                ri=0;
            }
            r=(BYTEread)(ri);
            data[3*x+y*wantedWidth]=b;
            data[3*x+y*wantedWidth+1]=g;
            data[3*x+y*wantedWidth+2]=r;
            //at this point the changed data is written to
        }
    }      

}
wait(0);
time_req = clock() - time_req;
printf("Time taken for forking: %f "
"seconds\n",
(float)time_req / CLOCKS_PER_SEC);

for (int y = 0; y < bitMapInfoHeader.biHeight; y++) {
        for (int x = 0; x < bitMapInfoHeader.biWidth; x++) {
           
            int test=3*x+y*wantedWidth;
            BYTEread b=data[3*x+y*wantedWidth]; //bgr format
            BYTEread g=data[3*x+y*wantedWidth+1];
            BYTEread r=data[3*x+y*wantedWidth+2];

            float bf=(float)b/255;
            float gf=(float)g/255;
            float rf=(float)r/255;
            //could add check
            float cgr= atof(r_grading_s.c_str());
            float cgg= atof(g_grading_s.c_str());
            float cgb= atof(b_grading_s.c_str());
                //multiplying normalized value by correction factor from terminal
            bf*=cgb;
            gf*=cgg;
            rf*=cgr;

            int bi=((int)(bf*255));
            if(bi>255){
                bi=255;
            }else if(bi<0){
                bi=0;
            }
            b=(BYTEread)(bi);

            int gi=((int)(gf*255));
            if(gi>255){
                gi=255;
            }else if(gi<0){
                gi=0;
            }
            g=(BYTEread)(gi);

            int ri=((int)(rf*255));
            if(ri>255){
                ri=255;
            }else if(ri<0){
                ri=0;
            }
            r=(BYTEread)(ri);
            data[3*x+y*wantedWidth]=b;
            data[3*x+y*wantedWidth+1]=g;
            data[3*x+y*wantedWidth+2]=r;
}
}

   wait(0);
    time_req = clock() - time_req;
    printf("Forking: %f "
           "seconds\n",
           (float)time_req / CLOCKS_PER_SEC);

FILE *OutFile = fopen(outputFile.c_str(), "wb"); //write back the file
fwrite(&bitMapFileHeader.bfType, sizeof(bitMapFileHeader.bfType), 1, OutFile);
fwrite(&bitMapFileHeader.bfSize, sizeof(bitMapFileHeader.bfSize), 1, OutFile);
fwrite(&bitMapFileHeader.bfReserved1, sizeof(bitMapFileHeader.bfReserved1), 1, OutFile);
fwrite(&bitMapFileHeader.bfReserved2, sizeof(bitMapFileHeader.bfReserved2), 1, OutFile);
fwrite(&bitMapFileHeader.bfOffBits, sizeof(bitMapFileHeader.bfOffBits), 1, OutFile);
fwrite(&bitMapInfoHeader, sizeof(tagBITMAPINFOHEADER), 1, OutFile);
fwrite(data, bitMapFileHeader.bfSize, 1, OutFile);
fclose(OutFile);


// clock_t duration = clock()-start;
// double durationSec = ((double)duration)/CLOCKS_PER_SEC;


// cout << "time with fork:"<<durationSec<<endl;

};
	
