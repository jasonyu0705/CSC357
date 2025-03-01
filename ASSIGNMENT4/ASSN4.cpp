#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <string.h>
#include <cmath>
#include <math.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
using namespace std;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

struct tagBITMAPFILEHEADER{
    WORD bfType; //specifies the file type
    DWORD bfSize; //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    //must be added seperately because there is padding 2 
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
    DWORD biClrUsed; //number of colors used by the bitmap
    DWORD biClrImportant; //number of colors that are important
};
struct HuffmanNode{
    int data;
    int freq;
    struct HuffmanNode *left, *right;
};
// struct FreqNode{
//     int data;
//     int freq;
//    // struct FreqNode *next, *prav;
// };
int compare_freq(const void *a, const void *b) {
    int idx1 = *(int*)a;  // Index (color value)
    int idx2 = *(int*)b;

    // Compare frequencies
    if (bList[idx1] != bList[idx2]) {
        return bList[idx1] - bList[idx2];  // Sort by frequency
    }
    
    return idx1 - idx2;  // If frequencies are equal, sort by color value
}

int main(int argc, char *argv[]){
    //taking in inputs from terminal
    string imageFile= argv[1];
    string quality= argv[2];

    //declaring struct values
    tagBITMAPFILEHEADER bmfh;
    tagBITMAPINFOHEADER bmih;
    BYTE* dataimg;

    //opening the file
    FILE* file=fopen(imageFile.c_str(),"rb");
    if (file==NULL){
        cout<<"FILE DOES NOT EXIST"<<endl;
        return 0;
    }
    //reading the first structs infomation (not mult of 4 data so we have to read one by one)
    fread(&bmfh.bfType,sizeof(bmfh.bfType),1,file);
    fread(&bmfh.bfSize,sizeof(bmfh.bfSize),1,file);
    fread(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,file);
    fread(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,file);
    fread(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,file);
    //reading second structs data
    fread(&bmih,sizeof(tagBITMAPINFOHEADER),1,file);
    //creating shared memory flag
    dataimg=(BYTE*)mmap(0,bmih.biSizeImage,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    fread(dataimg,bmih.biSizeImage,1,file);
    fclose(file);

    //correcting width with padding
    LONG correctWidth=3*bmih.biWidth;
    if (((bmih.biWidth)*3)%4!=0 ){
        correctWidth = 3* bmih.biWidth+ (4-((bmih.biWidth)*3)%4);
    } 

    //creating frequency lists
    int bList[256];
    int gList[256];
    int rList[256];


    //put default values in the frequency list
    for (int i = 0; i < 256; i++) {
        //bList[i].data = i;   
        bList[i] = 0; 
        //gList[i].data = i;   
        gList[i] = 0; 
        //rList[i].data = i;   
        rList[i] = 0; 
    }
    //this for lop is responsible for adding the data to the frequency list
    for (int y = 0; y < bmih.biHeight; y++) {
        for (int x = 0; x < bmih.biWidth; x++) {
            //this is for each pixel, then for each pixel we can say there are 3 colour valeus
            //getting each 
            BYTE bVal=dataimg[3*x+y*correctWidth];
            BYTE gVal=dataimg[3*x+y*correctWidth+1];
            BYTE rVal=dataimg[3*x+y*correctWidth+2];

            // add 1 to the frequency of the certain color level 
            bList[bVal]++;
            gList[gVal]++;
            rList[rVal]++;

        }
    }  
    // sort the data
    qsort(bList, 256, sizeof(int), compare);
    qsort(gList, 256, sizeof(int), compare);
    qsort(rList, 256, sizeof(int), compare);

    // //turn bytes into float values
    //         float bfVal=(float)bVal/255;
    //         float gfVal=(float)gVal/255;
    //         float rfVal=(float)rVal/255;
            
    //         //results 
    //         float bfr=0;
    //         float gfr=0;
    //         float rfr=0;

    //         //rewriting the data back into the image and capping it
    //         dataimg[3*x+y*correctWidth] = (BYTE)(fminf(fmaxf(bfr * 255, 0), 255));
    //         dataimg[3*x+y*correctWidth + 1] = (BYTE)(fminf(fmaxf(gfr * 255, 0), 255));
    //         dataimg[3*x+y*correctWidth + 2] = (BYTE)(fminf(fmaxf(rfr * 255, 0), 255));

    return 0;
}