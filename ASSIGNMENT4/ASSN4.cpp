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
    LONG redFreq[256];
    LONG greenFreq[256];
    LONG blueFreq[256];

};

struct HuffNode{
    int data;
    int freq;
    int il,ir;
    struct HuffNode *left, *right;
};


HuffNode *currentList;
// // Recursive function to free all nodes in a Huffman tree.
// void freeTree(HuffNode* root) {
//     if (!root) return;
//     freeTree(root->left);
//     freeTree(root->right);
//     delete root;
// }
// int compare(const void *ptr1, const void *ptr2){
//     const HuffNode* n1 = *(const HuffNode **)ptr1;
//     const HuffNode* n2 = *(const HuffNode **)ptr2;

//     if(n1->freq < n2->freq){
//         return -1;
//     }
//     else if(n1->freq == n2->freq){
//         if(n1->data < n2->data){
//             return -1;
//         }
//     }
//     else{
//         return 1;
//     }
// }
int compare(const void *ptr1, const void *ptr2) {
    const HuffNode* n1 = *(const HuffNode **)ptr1;
    const HuffNode* n2 = *(const HuffNode **)ptr2;
    if (n1->freq != n2->freq)
        return (n1->freq - n2->freq);
    return (n1->data - n2->data);
}
void preOrder(HuffNode* root, FILE* fileOut) {
    if (!root) return;

    // Write the node value (-1 for internal nodes)
    if (root->left == NULL && root->right == NULL) {
        int data = root->data;
        fwrite(&data, sizeof(int), 1, fileOut);  // Write leaf node data
    } else {
        int internalNodeMarker = -1;
        fwrite(&internalNodeMarker, sizeof(int), 1, fileOut);  // Internal node marker
    }

    // Recurse for left and right children
    preOrder(root->left, fileOut);
    preOrder(root->right, fileOut);
}
// Function to write a single bit into the packed data array
void writeBit(BYTE bit, BYTE packedData[], int *bitPos, int *bit_counter) {

    int byteIndex = *bitPos / 8;   
    
    int shift=7-*bit_counter;
    if (bit==1) {  // If bit is 1, set the bit at the correct position
     BYTE bitMask= 1<<shift;
        packedData[byteIndex] |= bitMask;
    }
    if(*bit_counter>=7){
        *bit_counter=0;
    }else{
        *bit_counter=*bit_counter+1;
    }
    *bitPos=*bitPos+1;
}

// Function to write a full Huffman code (string) into the packed data array
void packBit(string &huffCode, BYTE packedData[], int *bitPos, int *bit_counter) {
    for (int i = 0; i < huffCode.size(); i++) {
        char bit = huffCode[i];  // Get the character from the string
        writeBit(bit - '0', packedData, bitPos,bit_counter);
    }
}


// Function to build a Huffman tree for a specific color
HuffNode* buildTree(HuffNode *huff[], int *size) {
    // Sort non-null nodes first
    qsort(huff, *size, sizeof(HuffNode *), compare);

    // Build Huffman Tree
    while (*size > 1) {
        // Sort again to ensure smallest nodes are at the front
        qsort(huff, *size, sizeof(HuffNode *), compare);

        // Create a new internal node
        HuffNode *newNode = (HuffNode *)malloc(sizeof(HuffNode));
        newNode->freq = huff[0]->freq + huff[1]->freq;
        newNode->data = -1;  // Internal nodes don't store color values
        newNode->left = huff[0];
        newNode->right = huff[1];

        // Replace first two nodes with the new merged node
        huff[0] = newNode;

        // Shift array left to remove second merged node
        for (int i = 1; i < *size - 1; i++) {
            huff[i] = huff[i + 1];
        }

        (*size)--;  // Reduce the list size
    }

    return huff[0];  // Return root of Huffman tree
}
//function that traverses the tree and creates codes for the values
void generateCode(HuffNode* root, string code,long length[], string huffData[]) {
    if (!root) return;
    
    // Leaf node: store the Huffman code
    if (root->left == NULL && root->right == NULL) {
        huffData[root->data] = code;
        length[root->data] = code.length();

    }
    
    // Recursively go left (add 0) and right (add 1)
    generateCode(root->left, code + "0",length, huffData);
    generateCode(root->right, code + "1", length, huffData);
}

int main(int argc, char *argv[]){
    //taking in inputs from terminal
    // string imageFile= argv[1];
    // string quality= argv[2];

    string imageFile= "jar.bmp";
    string quality= "10";
    string OutputFile= "aaa.zzz";
    //declaring struct values
    tagBITMAPFILEHEADER bmfh;
    tagBITMAPINFOHEADER bmih;
    tagCOMPRESSHEADER CH;

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
    HuffNode* bList[256];
    HuffNode* gList[256];
    HuffNode* rList[256];
    string rHuffCodes[256], gHuffCodes[256], bHuffCodes[256];
    long rHuffLen[256], gHuffLen[256], bHuffLen[256];

    BYTE packedRed[bmih.biSizeImage];
    BYTE packedGreen[bmih.biSizeImage];
    BYTE packedBlue[bmih.biSizeImage];

    //initializing lists to 0
    memset(rHuffCodes, 0, 256);
    memset(gHuffCodes, 0,256);
    memset(bHuffCodes, 0, 256);
    memset(gHuffLen, 0, 256);
    memset(rHuffLen, 0, 256);
    memset(bHuffLen, 0, 256);

    //creating size counters for freqwuency noirtes

    //put default values in the frequency list
    for (int i = 0; i < 256; i++) {
        bList[i]= 0;   
        gList[i]= 0;   
        rList[i]= 0;   
        CH.redFreq[i] = 0;
        CH.greenFreq[i] = 0;
        CH.blueFreq[i] = 0;
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
            if(bList[bVal]==NULL){
                bList[bVal]=new HuffNode;
                bList[bVal]->data=bVal;
                bList[bVal]->freq=0;
                bList[bVal]->left = NULL;
                bList[bVal]->right = NULL;

            }
            bList[bVal]->freq++;

            if(gList[gVal]==NULL){
                gList[gVal]=new HuffNode;
                gList[gVal]->data=gVal;
                gList[gVal]->freq=0;
                gList[gVal]->left = NULL;
                gList[gVal]->right = NULL;
            }
            gList[gVal]->freq++;

            if(rList[rVal]==NULL){
                rList[rVal]=new HuffNode;
                rList[rVal]->data=rVal;
                rList[rVal]->freq=0;
                rList[rVal]->left = NULL;
                rList[rVal]->right = NULL;
            }
            rList[rVal]->freq++;
        }
    }  

    // Count the number of non-null Huffman nodes
    // Declare size counters for each list
    int rSize = 0, gSize = 0, bSize = 0;
    for (int i = 0; i < 256; i++) {
        if (bList[i] != NULL) {
            bList[bSize] = bList[i];  // Move non-null nodes to the front
            bSize++;
        }
        if (gList[i] != NULL) {
            gList[gSize] = gList[i];  // Move non-null nodes to the front
            gSize++;
        }
        if (rList[i] != NULL) {
            rList[rSize] = rList[i];  // Move non-null nodes to the front
            rSize++;
        }
    }
    for (int i = 0; i < bSize; i++) {
        int val = bList[i]->data;
        CH.blueFreq[i] = bList[i]->freq;
    }
    for (int i = 0; i < gSize; i++) {
             int val = gList[i]->data;
        CH.greenFreq[i] = gList[i]->freq;
    }
    for (int i = 0; i < rSize; i++) {
         int val = rList[i]->data;
        CH.redFreq[i] = rList[i]->freq;
    }

    //build the trees
    HuffNode *rRoot =buildTree(rList, &rSize);
    HuffNode *gRoot =buildTree(gList, &gSize);
    HuffNode *bRoot =buildTree(bList, &bSize);

    //get the huffman codes from the tree
    generateCode(rRoot, "",rHuffLen, rHuffCodes);
    generateCode(gRoot, "",gHuffLen,gHuffCodes);
    generateCode(bRoot, "",bHuffLen, bHuffCodes);
 
   //bit position poinrters
    int bitPosRed = 0, bitPosGreen = 0, bitPosBlue = 0;
    int bit_counter;

    bit_counter=0;
    //convert this to huffman data
    for (int y = 0; y <  bmih.biHeight; y++) {  // Loop through rows
        for (int x = 0; x <  bmih.biWidth; x++) {  // Loop through columns
            // Extract RGB values from the image data array
            BYTE bVal = dataimg[3 * x + y * correctWidth];      // Blue
            BYTE gVal = dataimg[3 * x + y * correctWidth + 1];  // Green
            BYTE rVal = dataimg[3 * x + y * correctWidth + 2];  // Red

            // Pack Huffman-encoded bits directly into unsigned char arrays
            packBit(bHuffCodes[bVal], packedBlue, &bitPosBlue,&bit_counter);
            packBit(gHuffCodes[gVal], packedGreen, &bitPosGreen, &bit_counter);
            packBit(rHuffCodes[rVal], packedRed, &bitPosRed,&bit_counter); 

         
        }

    }

    //at this point all the data should be packed and is ready to be written to the file
    //writing the file
    FILE* fileOut=fopen(OutputFile.c_str(),"wb");
    //putting putting things in the file header
    CH.headerSize = sizeof(tagCOMPRESSHEADER);
    CH.dataOffset = sizeof(tagBITMAPFILEHEADER) + sizeof(tagBITMAPINFOHEADER) + sizeof(tagCOMPRESSHEADER);
    // putting things in the info header
    CH.redSize = rSize;
    CH.greenSize = gSize;
    CH.blueSize = bSize;
    CH.width = bmih.biWidth;
    CH.height = bmih.biHeight;
 

    fwrite(&bmfh.bfType,sizeof(bmfh.bfType),1,fileOut);
    fwrite(&bmfh.bfSize,sizeof(bmfh.bfSize),1,fileOut);
    fwrite(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,fileOut);
    fwrite(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,fileOut);
    fwrite(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,fileOut);

    fwrite(&bmih,sizeof(tagBITMAPINFOHEADER),1,fileOut);
    fwrite(&CH, sizeof(tagCOMPRESSHEADER), 1, fileOut);

    preOrder(rRoot, fileOut);
    preOrder(gRoot, fileOut);
    preOrder(bRoot, fileOut);

    fwrite(packedRed, sizeof(BYTE), CH.redSize, fileOut);
    fwrite(packedGreen, sizeof(BYTE), CH.greenSize, fileOut);
    fwrite(packedBlue, sizeof(BYTE), CH.blueSize, fileOut);
    fclose(fileOut);

    //munmap(dataimg, bmih.biSizeImage);

    return 0;
}