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
#include <vector>
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

// InfoHeader stores metadata required for decompression
struct tagCOMPRESSHEADER {
    DWORD headerSize;// Size of the info header.
    DWORD dataOffset;// Offset from the beginning of the file to the compressed data.
    LONG width;// Original image width.
    LONG height;// Original image height.
    LONG redSize;// Compressed size (in bytes) for the red channel.
    LONG greenSize;// Compressed size (in bytes) for the green channel.
    LONG blueSize;// Compressed size (in bytes) for the blue channel.
};

struct HuffNode{
    int data;
    int freq;
    struct HuffNode *left, *right;
};


HuffNode *currentList;
void freeHuffTree(HuffNode* node) {
    if (!node) return;
    freeHuffTree(node->left);
    freeHuffTree(node->right);
    delete node;
}

HuffNode* postOrder(FILE* file) {
    int value;
    fread(&value, sizeof(int), 1, file); // Read value from file and check for EOF

    // If it's a leaf node (not -1), create and return a leaf HuffNode
    if (value != -1) {
        HuffNode* leaf = new HuffNode;
        leaf->data = value;
        leaf->freq = 0; 
        leaf->left = NULL;
        leaf->right = NULL;
        return leaf;
    }

    // If it's an internal node (-1), create a new node and recursively build left & right subtrees
    HuffNode* node = new HuffNode;
    node->data = -1;  
    node->freq = 0; 

    node->left = postOrder(file);
    node->right = postOrder(file);

    return node;
}

int decode(HuffNode* root, BYTE* bitstring, int* bitPos, int bitLength,int *bitCount) {
    HuffNode* current = root;
    while (current) {
        //leafnode 
        if (current->data != -1) {
            return current->data;  
        }

        // Ensure we don't go out of bounds in the bitstring
        if (*bitPos < bitLength) {

            int bytePos = *bitPos / 8;       // Determine byte position
            //int bit = (bitstring[bytePos] >> (7-*bitPos)) & 1; 
            int shift=  7 - *bitCount;
            int bit = (bitstring[bytePos] >>shift) & 1;  

        // int byteIndex = *bitPos / 8;   
    
        // int shift=7-*bitCount;
        // BYTE bitMask= 1<<shift;

            if (bit == 0) {
                current = current->left;
            } else {
                current = current->right;
            }

            if(*bitCount>=7){
                *bitCount=0;
            }else{
                *bitCount=*bitCount+1;
            }
            *bitPos=*bitPos+1;
            
        } 
    }

    return 100000;  
}


int main(int argc, char *argv[]){
    //taking in inputs from terminal
    // string imageFile= argv[1];
    // string quality= argv[2];

    string imageFile= "yiy.zzz";
    string quality= "1";
    string OutputFile= "tester.bmp";
    //declaring struct values
    tagBITMAPFILEHEADER bmfh;
    tagBITMAPINFOHEADER bmih;
    tagCOMPRESSHEADER CH;

    BYTE *redData;
    BYTE *greenData;
    BYTE *blueData;

    //opening the file
    FILE* file=fopen(imageFile.c_str(),"rb");
    if (file==NULL){
        cout<<"FILE DOES NOT EXIST"<<endl;
        return 0;
    }
    //READING LOGIC IS DIFFERENT BECAUSE WE ARE READING IN A DIFFERENT FILE TYPE
    //reading the first structs infomation (not mult of 4 data so we have to read one by one)
    fread(&bmfh.bfType,sizeof(bmfh.bfType),1,file);
    fread(&bmfh.bfSize,sizeof(bmfh.bfSize),1,file);
    fread(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,file);
    fread(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,file);
    fread(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,file);

    fread(&bmih,sizeof(tagBITMAPINFOHEADER),1,file);
    fread(&CH, sizeof(tagCOMPRESSHEADER), 1, file);
    //the actual data
    redData=(BYTE*)mmap(0,CH.redSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    greenData=(BYTE*)mmap(0,CH.greenSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    blueData=(BYTE*)mmap(0,CH.blueSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);


    HuffNode* rRoot = postOrder(file);
    HuffNode* gRoot = postOrder(file);
    HuffNode* bRoot = postOrder(file);

    fread(blueData, sizeof(BYTE), CH.blueSize, file);
    fread(greenData, sizeof(BYTE), CH.greenSize, file);
    fread(redData, sizeof(BYTE), CH.redSize, file);

    fclose(file);
    //creating frequency lists
    string rHuffCodes[256], gHuffCodes[256], bHuffCodes[256];
    long rHuffLen[256], gHuffLen[256], bHuffLen[256];
    BYTE* dataimg=(BYTE*)mmap(0,bmih.biSizeImage,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    //initializing lists to 0
for (int i = 0; i < 256; i++) {
    rHuffCodes[i] = "";
    gHuffCodes[i] = "";
    bHuffCodes[i] = "";
    rHuffLen[i] = 0;
    gHuffLen[i] = 0;
    bHuffLen[i] = 0;
}
    
//switch everything to otehr width
    LONG correctWidth=3*CH.width;
    if (((CH.width)*3)%4!=0 ){
        correctWidth = 3* CH.width+ (4-((CH.width)*3)%4);
    }

    long bIndex = 0, gIndex = 0, rIndex = 0;  // Track positions in the bitstream
    int bitPosRed = 0, bitPosGreen = 0, bitPosBlue = 0;
    int rBitCount,bBitCount,gBitCount;//0 to 7
    rBitCount=0;
    bBitCount=0;
    gBitCount=0;
// Decode blue channel
// int bitLength = CH.blueSize * 8;  // Total number of bits in blue channel

// Decode and write to image data array directly
for (int y = 0; y < CH.height; y++) {  
    for (int x = 0; x < CH.width; x++) {  
        int bVal = decode(bRoot, blueData, &bitPosBlue, CH.blueSize * 8,&bBitCount);
        int gVal = decode(gRoot, greenData, &bitPosGreen, CH.greenSize * 8,&gBitCount);
        int rVal = decode(rRoot, redData, &bitPosRed, CH.redSize * 8,&rBitCount);

        if (bVal == -1 || gVal == -1 || rVal == -1) {
            cout << "Error decoding pixel at (" << x << "," << y << ")" << endl;
            return 1;
        }

        // Write decoded values into `dataimg`
        dataimg[3*x+y*correctWidth] = (BYTE) (fminf(fmaxf(bVal * 255, 0), 255));
        dataimg[3*x+y*correctWidth+1] =(BYTE) (fminf(fmaxf(gVal * 255, 0), 255));
        dataimg[3*x+y*correctWidth+2] = (BYTE) (fminf(fmaxf(rVal * 255, 0), 255));
    }
}

 FILE* fileOut=fopen(OutputFile.c_str(),"wb");
 //reading the first structs infomation (not mult of 4 data so we have to read one by one)
    fwrite(&bmfh.bfType,sizeof(bmfh.bfType),1,fileOut);
    fwrite(&bmfh.bfSize,sizeof(bmfh.bfSize),1,fileOut);
    fwrite(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,fileOut);
    fwrite(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,fileOut);
    fwrite(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,fileOut);
    //reading second structs data
    fwrite(&bmih,sizeof(tagBITMAPINFOHEADER),1,fileOut);
    fwrite(dataimg,bmih.biSizeImage,1,fileOut);
    fclose(fileOut);


// Clean up Huffman trees
    freeHuffTree(rRoot);
    freeHuffTree(gRoot);
    freeHuffTree(bRoot);
    munmap(redData, CH.redSize);
    munmap(greenData, CH.greenSize);
    munmap(blueData, CH.blueSize);
    munmap(dataimg, bmih.biSizeImage);
    return 0;
}
 