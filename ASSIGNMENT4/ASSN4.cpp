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
struct HuffNode{
    int data;
    int freq;
    struct HuffNode *left, *right;
};


HuffNode *currentList;

int compare(const void *ptr1, const void *ptr2){
    const HuffNode* n1 = *(const HuffNode **)ptr1;
    const HuffNode* n2 = *(const HuffNode **)ptr2;

    if(n1->freq < n2->freq){
        return -1;
    }
    else if(n1->freq == n2->freq){
        if(n1->data < n2->data){
            return -1;
        }
    }
    else{
        return 1;
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
    string quality= "1";

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
    HuffNode* bList[256];
    HuffNode* gList[256];
    HuffNode* rList[256];
    string rHuffCodes[256], gHuffCodes[256], bHuffCodes[256];
    long rHuffLen[256], gHuffLen[256], bHuffLen[256];
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

    //build the trees
    HuffNode *rRoot =buildTree(rList, &rSize);
    HuffNode *gRoot =buildTree(gList, &gSize);
    HuffNode *bRoot =buildTree(bList, &bSize);

    //get the huffman codes from the tree
    generateCode(rRoot, "",rHuffLen, rHuffCodes);
    generateCode(gRoot, "",gHuffLen,gHuffCodes);
    generateCode(bRoot, "",bHuffLen, bHuffCodes);
    //packing bits into data





    // // sort the data
    // currentList = bList;  // Set global pointer to the blue list
    // qsort(bList, 256, sizeof(HuffNode*), compare);
    // currentList = gList;  // Set global pointer to the green list
    // qsort(gList, 256, sizeof(HuffNode*), compare);
    // currentList = rList;  // Set global pointer to the red list
    // qsort(rList, 256, sizeof(HuffNode*), compare);
    // //data is sorted now we can create the huffman tree

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
    //huffman tree
    //already in data
    // through loops in data array and rextracting colours
    //after fors sort it 
    //christian said use sort but i s8used qsort
    //MUST USE TUPLE FOR SORT OR SOME SORT OF STRUCT
    //take top 2 and build parent out of it
    //sort after each iteration fo the huffman tree



    return 0;
}