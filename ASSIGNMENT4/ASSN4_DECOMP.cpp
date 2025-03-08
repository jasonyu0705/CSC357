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
    struct HuffNode *left, *right;
};


HuffNode *currentList;

int compare(const void *ptr1, const void *ptr2) {
    const HuffNode* n1 = *(const HuffNode **)ptr1;
    const HuffNode* n2 = *(const HuffNode **)ptr2;
    if (n1->freq != n2->freq)
        return (n1->freq - n2->freq);
    return (n1->data - n2->data);
}

HuffNode* postOrder(FILE* file) {
    int value;
    if (fread(&value, sizeof(int), 1, file) != 1) return NULL; // Read value from file and check for EOF

    // If it's a leaf node (not -1), create and return a leaf HuffNode
    if (value != -1) {
        HuffNode* leaf = new HuffNode;
        leaf->data = value;
        leaf->freq = 0; // Frequency is unknown during deserialization
        leaf->left = NULL;
        leaf->right = NULL;
        return leaf;
    }

    // If it's an internal node (-1), create a new node and recursively build left & right subtrees
    HuffNode* node = new HuffNode;
    node->data = -1;  // Internal node marker
    node->freq = 0;   // Frequency is unknown during deserialization

    node->left = postOrder(file);
    node->right = postOrder(file);

    return node;
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

    string imageFile= "aaa.zzz";
    string quality= "1";
    string OutputFile= "test.bmp";
    //declaring struct values
    tagBITMAPFILEHEADER bmfh;
    tagBITMAPINFOHEADER bmih;
    tagCOMPRESSHEADER CH;

    BYTE *redData;
    BYTE *greenData;
    BYTE *blueData;
vector<int> serializedRedTree, serializedGreenTree, serializedBlueTree;

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

    // serializedRedTree.resize(521);
    // fread(serializedRedTree.data(), sizeof(int), serializedRedTree.size(), file);
    // serializedGreenTree.resize(512);
    // fread(serializedGreenTree.data(), sizeof(int), serializedGreenTree.size(), file);
    // serializedBlueTree.resize(512);
    // fread(serializedBlueTree.data(), sizeof(int), serializedBlueTree.size(), file);  
    HuffNode* rRoot = postOrder(file);
    HuffNode* gRoot = postOrder(file);
    HuffNode* bRoot = postOrder(file);

    fwrite(redData, sizeof(BYTE), CH.redSize, file);
    fwrite(greenData, sizeof(BYTE), CH.greenSize, file);
    fwrite(blueData, sizeof(BYTE), CH.blueSize, file);

    fclose(file);

    //correcting width with padding
    //DONT THINK I NEED THIS BECAUSe I AM READING IN A DIFFERENT FILE TYPE BUT MAYBE NEED TO ADD PADDIGN BACK LATER 
    // LONG correctWidth=3*bmih.biWidth;
    // if (((bmih.biWidth)*3)%4!=0 ){
    //     correctWidth = 3* bmih.biWidth+ (4-((bmih.biWidth)*3)%4);
    // } 

    //creating frequency lists
    string rHuffCodes[256], gHuffCodes[256], bHuffCodes[256];
    long rHuffLen[256], gHuffLen[256], bHuffLen[256];

    //initializing lists to 0
    memset(rHuffCodes, 0, 256);
    memset(gHuffCodes, 0,256);
    memset(bHuffCodes, 0, 256);
    memset(gHuffLen, 0, 256);
    memset(rHuffLen, 0, 256);
    memset(bHuffLen, 0, 256);

     // --- Integrated Frequency List and Huffman Tree Reconstruction ---
    // Create arrays to hold pointers to HuffNodes for each color channel.
    HuffNode* bList_out[256];
    HuffNode* gList_out[256];
    HuffNode* rList_out[256];
    int rSize_out = 0, gSize_out = 0, bSize_out = 0;
    // populating node list
    for (int i = 0; i < 256; i++) {
         if (CH.redFreq[i] > 0) {
             HuffNode* node = new HuffNode;
             node->data = i;
             node->freq = CH.redFreq[i];
             node->left = node->right = nullptr;
             bList_out[rSize_out++] = node;
         }
         if (CH.greenFreq[i] > 0) {
             HuffNode* node = new HuffNode;
             node->data = i;
             node->freq = CH.greenFreq[i];
             node->left = node->right = nullptr;
             gList_out[gSize_out++] = node;
         }
         if (CH.blueFreq[i] > 0) {
             HuffNode* node = new HuffNode;
             node->data = i;
             node->freq = CH.blueFreq[i];
             node->left = node->right = nullptr;
             rList_out[bSize_out++] = node;
         }
    }
int redIndex = 0, greenIndex = 0, blueIndex = 0;

    //get the huffman codes from the tree
    generateCode(rRoot, "",rHuffLen, rHuffCodes);
    generateCode(gRoot, "",gHuffLen,gHuffCodes);
    generateCode(bRoot, "",bHuffLen, bHuffCodes);
    return 0;
}