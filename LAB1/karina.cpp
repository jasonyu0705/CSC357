#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <string.h>
// #include <cstdlib>
// #include <iostream>
// #include <bits/stdc++.h>
// #include <string.h>
// #include <string>
// #include <cmath>
// #include <math.h>
// #include <cstdlib>


using namespace std;

   //./image is the program -> face.bmp is the file read
    // read the file with (data, bin.size, 1, file)
    //read the file name in the command line with the parameters
    //also translate it to the header,ensure sizing is the same
    //be able to output the file (result) - result.face.bmp: example and end with the float after read in (result)
    //inside what is the operation that we are doing to the file using bitmapping  
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


int main(int argc, char*argv[]){ //argv parse command line arguments, argc = # of arguments
    // if (argc < 5) {
    // cout << "Error: Insufficient arguments provided." << endl;
    // cout << "Usage: ./program <imageFile> <outputFile> <operation> <contrastFactor>" << endl;
    // return 1;
    // }
    //take in the arguments - input , [program name, imagefile, outputfile, operation, contrast factor]
    string imageFile = argv[1];
    string outputFile = argv[2];
    string operation = argv[3];
    string contrastFactor = argv[4];
    // string imageFile = "jar.bmp";
    // string outputFile = "output.bmp";
    // string operation = "contrast";
    // string contrastFactor = "1.5";

    tagBITMAPFILEHEADER bitMapFileHeader; //call the struct
    tagBITMAPINFOHEADER bitMapInfoHeader; //call the struct
    // could use std to show that it is referenced to just that

 
    // //take input for the file name !!!!
    // std::string filename = "input.txt";
    // std::ifstream inputFile("input.txt"); //opens the file for reading
    // if(!inputFile.is_open()){
    //     // std::perror << "Could not open file" <<
    // }
    cout << "Image File: " << imageFile << endl;
    cout << "Output File: " << outputFile << endl;
    cout << "Operation: " << operation << endl;
    cout << "Contrast Factor: " << contrastFactor << endl;


    // for (row and height){ nested for loop for the pixels
    //     for(){

    //     }
    // }
    BYTEread* data; // read the data

    //FILE *fopen(const char *restrict pathname, const char *restrict mode);
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
    data=(BYTEread*)malloc(bitMapFileHeader.bfSize);

    fread(data, bitMapFileHeader.bfSize, 1, fileIn);
    fclose(fileIn); // then close the file
    LONG wantedWidth=3*bitMapInfoHeader.biWidth+(((bitMapInfoHeader.biWidth)*3)%4); //dealing w multiple of 4

    //choose the opertion will each iterte throught
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
            float cf= atof(contrastFactor.c_str());
            if (strcmp(operation.c_str(), "contrast") == 0) {
                bf=pow(bf,cf/10);
                gf=pow(gf,cf/10);
                rf=pow(rf,cf/10);
                //Contrast: Pixel = pow ( Pixel , factor);
            }else if(strcmp(operation.c_str(), "saturation") == 0){
                float avg= (bf+gf+rf)/3;
                bf=bf+(bf-avg)*cf;
                gf=gf+(gf-avg)*cf;
                rf=rf+(rf-avg)*cf;
            //Pixel (color) = Pixel (color) + (Pixel (color) – Pixel (average)) * factor;
            }else if(strcmp(operation.c_str(), "lightness") == 0){
                bf=bf+cf;
                gf=gf+cf;
                rf=rf+cf;
            //Lightness: Pixel (color) = Pixel (color) + factor;
            }
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


//  Contrast: Pixel = pow ( Pixel , factor); //factor [0,100]
// Saturation: Pixel (co(lor) = Pixel (color) + (Pixel (color) – Pixel (average)) * factor; //factor [0,1]
// Lightness: Pixel (color) = Pixel (color) + factor; //factor [-1,1]
    FILE *OutFile = fopen(outputFile.c_str(), "wb"); //write back the file
    fwrite(&bitMapFileHeader.bfType, sizeof(bitMapFileHeader.bfType), 1, OutFile);
    fwrite(&bitMapFileHeader.bfSize, sizeof(bitMapFileHeader.bfSize), 1, OutFile);
    fwrite(&bitMapFileHeader.bfReserved1, sizeof(bitMapFileHeader.bfReserved1), 1, OutFile);
    fwrite(&bitMapFileHeader.bfReserved2, sizeof(bitMapFileHeader.bfReserved2), 1, OutFile);
    fwrite(&bitMapFileHeader.bfOffBits, sizeof(bitMapFileHeader.bfOffBits), 1, OutFile);

    fwrite(&bitMapInfoHeader, sizeof(tagBITMAPINFOHEADER), 1, OutFile);
    fwrite(data, bitMapFileHeader.bfSize, 1, OutFile);
    fclose(OutFile);

    free(data);
    return 0;
};

//bit mapping starts with 42 4D --> sizing (little endian and big endian to determine size)-> reserved -> offset -> go to offset and that's
//where everything in the header starts (has to be divisible by 4) FF FF FF - white(1 pixel) (each FF is 8 bits)

//header:
//parsing and to change the bits (saturation, contrasting, brightness), for each bit probably use a for loop to iterate