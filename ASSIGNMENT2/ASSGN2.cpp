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

//-------------------------------------------------------------------------------------------------------------------------------------
// unsigned char Bilinear(unsigned char *imagedata, float x, float y, int imagewidth, int imageheight) {
//     int x1 = (int)x;
//     int y1 = (int)y;
//     int x2 = x1 + 1;
//     int y2 = y1 + 1;

//     // Boundary check
//     if (x1 >= imagewidth) x1 = imagewidth - 1;
//     if (y1 >= imageheight) y1 = imageheight - 1;
//     if (x2 >= imagewidth) x2 = imagewidth - 1;
//     if (y2 >= imageheight) y2 = imageheight - 1;

//     float dx = x - x1;
//     float dy = y - y1;

//     // BMP is stored as BGR, so red is at index 2
//     int stride = imagewidth * 3; // Row stride in bytes

//     unsigned char tl = imagedata[y2 * stride + x1 * 3 + 2];
//     unsigned char tr = imagedata[y2 * stride + x2 * 3 + 2];
//     unsigned char bl = imagedata[y1 * stride + x1 * 3 + 2];
//     unsigned char br = imagedata[y1 * stride + x2 * 3 + 2];

//     // Bilinear interpolation
//     float Red_left = Red_left_upper * (1 - dy) + Red_left_lower * dy;
//     float Red_right = Red_right_upper * (1 - dy) + Red_right_lower * dy;
//     float Red_result = Red_left * (1 - dx) + Red_right * dx;

//     return (unsigned char)Red_result;
// }
BYTE get_color_bilinear(tagBITMAPINFOHEADER ih,BYTE *imagedata, float x, float y, int imagewidth, int imageheight, int colour) {
    //finding x1 and x2 given the lab format
    int x1 = floor(x);
    int y1 = floor(y);
    int x2 = x1 + 1;
    int y2 = y1 + 1;

    // Clamp coordinates so they lie within the image bounds
    if (x1 < 0) x1 = 0;
    if (y1 < 0) y1 = 0;
    if (x2 >= imagewidth) x2 = imagewidth - 1;
    if (y2 >= imageheight) y2 = imageheight - 1;

    float dx = x - x1;
    float dy = y - y1;

    int rowsize = 3* ih.biWidth+(((ih.biWidth)*3)%4);
    //int rowsize = 3*imagewidth;


    // Get the four neighboring pixels for the given colour
    BYTE tl  = imagedata[(y2 * rowsize + x1 * 3) + colour];
    BYTE tr = imagedata[(y2 * rowsize + x2 * 3) + colour];
    BYTE bl  = imagedata[(y1 * rowsize + x1 * 3) + colour];
    BYTE br = imagedata[(y1 * rowsize + x2 * 3) + colour];

    // First interpolate vertically
    float left  = tl * (dy) + bl * (1-dy);
    float right = tr * (dy) + br * (1-dy);

    // Then interpolate horizontally
    return (BYTE) left * (1 - dx) + right * dx;
}

pid_t pid;
pid_t pid_kid;
int larger;


int main(int argc,char* argv[]){
     clock_t time_req;
    time_req = clock();

    string imageFileOne= argv[1];
    string imageFileTwo= argv[2];
    string ratio= argv[3];
    string process_number= argv[4];
    string OutputFile= argv[5];

    // string imageFileOne= "Mario.bmp";
    // string imageFileTwo= "jar.bmp";
    // string ratio= "0.5";
    // string process_number= "3";
    // string OutputFile= "test2.bmp";

    if(argc!=5){
        cout<<"the following is the format:"<<endl;
        cout<<"[programname] [imagefile1] [imagefile2] [ratio] [process#] [outputfile]"<<endl;
        
    }
    //initialize the pointers to struct fro the first image and the second image
    tagBITMAPFILEHEADER bmfh1;
    tagBITMAPINFOHEADER bmih1;
    tagBITMAPFILEHEADER bmfh2;
    tagBITMAPINFOHEADER bmih2;
    tagBITMAPINFOHEADER bmih_large;
    tagBITMAPFILEHEADER bmfh_large;
    BYTE* dataimg1;
    BYTE* dataimg2;

    //OPENING FILE ONE INFOMATION-------------------------------------------------------------------------------------------
    //opening the file
    FILE* file1=fopen(imageFileOne.c_str(),"rb");
    if (file1==NULL){
        cout<<"NOT EXIST"<<endl;
    }
    //reading the first structs infomation (not mult of 4 data so we have to read one by one)
    fread(&bmfh1.bfType,sizeof(bmfh1.bfType),1,file1);
    fread(&bmfh1.bfSize,sizeof(bmfh1.bfSize),1,file1);
    fread(&bmfh1.bfReserved1,sizeof(bmfh1.bfReserved1),1,file1);
    fread(&bmfh1.bfReserved2,sizeof(bmfh1.bfReserved2),1,file1);
    fread(&bmfh1.bfOffBits,sizeof(bmfh1.bfOffBits),1,file1);
    //reading second structs data
    fread(&bmih1,sizeof(tagBITMAPINFOHEADER),1,file1);
    //creating shared memory flag
    dataimg1=(BYTE*)mmap(0,bmih1.biSizeImage,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    fread(dataimg1,bmih1.biSizeImage,1,file1);
    fclose(file1);
//-------------------------------------------------------------------------------------------------------------------------------------
//OPENING FILE TWO INFOMATION-------------------------------------------------------------------------------------------
    //opening the file
    FILE* file2=fopen(imageFileTwo.c_str(),"rb");
    if (file2==NULL){
        cout<<"NOT EXIST"<<endl;
    }
    //reading the first structs infomation (not mult of 4 data so we have to read one by one)
    fread(&bmfh2.bfType,sizeof(bmfh2.bfType),1,file2);
    fread(&bmfh2.bfSize,sizeof(bmfh2.bfSize),1,file2);
    fread(&bmfh2.bfReserved1,sizeof(bmfh2.bfReserved1),1,file2);
    fread(&bmfh2.bfReserved2,sizeof(bmfh2.bfReserved2),1,file2);
    fread(&bmfh2.bfOffBits,sizeof(bmfh2.bfOffBits),1,file2);
    //reading second structs data
    fread(&bmih2,sizeof(tagBITMAPINFOHEADER),1,file2);
    //creating shared memory flag
    //could be bi size image
    dataimg2=(BYTE*)mmap(0,bmih2.biSizeImage,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    fread(dataimg2,bmih2.biSizeImage,1,file2);

    fclose(file2);
//-------------------------------------------------------------------------------------------------------------------------------------
//should onkly have one corrected width
    // LONG correctWidth1=3*bmih1.biWidth+(((bmih1.biWidth)*3)%4);
    // LONG correctWidth2=3*bmih2.biWidth+(((bmih2.biWidth)*3)%4);


    //ALSO DECIDING WHICH IMAGE HAS THE HGHER RESOLUTION 
    //bilinear also goes here
    if(bmih1.biWidth<=bmih2.biWidth){//bilinear on 1
        bmih_large=bmih2;
        larger= 1;

    }else if(bmih1.biWidth>bmih2.biWidth){//bilinear on 2
        bmih_large=bmih1;
        larger= 0;
    }
    LONG correctWidth=3*bmih_large.biWidth+(((bmih_large.biWidth)*3)%4);

    //create the correct number of forks by saying that you will keep creating forks while the process number is not reched
    //and the process is not the parents

    for (int x = 0; x <= stoi(process_number)-1 && pid==0; x++) {
        pid_kid=fork();

        if (pid_kid < 0) {
        perror("Fork failed");
        exit(1);
        }   

        if(pid_kid==0){
            //create a kid and if we are in the kid
            LONG process_height=bmih_large.biHeight/stoi(process_number);
            int fork_bottom=x*process_height;
            //can be -1 
            int fork_top=(x==stoi(process_number))? bmih_large.biHeight : fork_bottom+ process_height;
            printf("process %d: Handling rows %ld to %ld\n",x,fork_bottom,fork_top);
            //iterates through the section of the photo
            
            for (int y = fork_bottom; y < fork_top; y++) {
                for (int x = 0; x < bmih_large.biWidth; x++) {
                    //getting the data from the photo and turning it into float form
                    LONG correctWidth1=3*bmih1.biWidth+(((bmih1.biWidth)*3)%4);
                    BYTE b1=dataimg1[3*x+y*correctWidth1];
                    BYTE g1=dataimg1[3*x+y*correctWidth1+1];
                    BYTE r1=dataimg1[3*x+y*correctWidth1+2];
                    float bf1=(float)b1/255;
                    float gf1=(float)g1/255;
                    float rf1=(float)r1/255;
                    
                    LONG correctWidth2=3*bmih2.biWidth+(((bmih2.biWidth)*3)%4);
                    BYTE b2=dataimg2[3*x+y*correctWidth2];
                    BYTE g2=dataimg2[3*x+y*correctWidth2+1];
                    BYTE r2=dataimg2[3*x+y*correctWidth2+2];
                    float bf2=(float)b2/255;
                    float gf2=(float)g2/255;
                    float rf2=(float)r2/255;
                    //results 
                    float bfr=0;
                    float gfr=0;
                    float rfr=0;
                    float float_ratio= stof(ratio);
                    //colour manipulation
                    if (larger==1){//image 2 is larger and therefore is the first one
                        // Map coordinates from image2 space to image1 space
                        float mapped_x = x * ((float)bmih1.biWidth / bmih_large.biWidth);
                        float mapped_y = y * ((float)bmih1.biHeight / bmih_large.biHeight);

                        BYTE b1 = get_color_bilinear(bmih1,dataimg1, mapped_x, mapped_y, bmih1.biWidth, bmih1.biHeight, 0);
                        BYTE g1 = get_color_bilinear(bmih1,dataimg1, mapped_x, mapped_y, bmih1.biWidth, bmih1.biHeight, 1);
                        BYTE r1 = get_color_bilinear(bmih1,dataimg1, mapped_x, mapped_y, bmih1.biWidth, bmih1.biHeight, 2);

                        float bf1 = (float) b1 / 255.0;
                        float gf1 = (float) g1 / 255.0;
                        float rf1 = (float) r1 / 255.0;

                            bfr=bf2*float_ratio+bf1*(1-float_ratio);
                            gfr=gf2*float_ratio+gf1*(1-float_ratio);
                            rfr=rf2*float_ratio+rf1*(1-float_ratio);
                
                        dataimg2[3*x+y*correctWidth] = (BYTE)(fminf(fmaxf(bfr * 255, 0), 255));
                        dataimg2[3*x+y*correctWidth + 1] = (BYTE)(fminf(fmaxf(gfr * 255, 0), 255));
                        dataimg2[3*x+y*correctWidth + 2] = (BYTE)(fminf(fmaxf(rfr * 255, 0), 255));

                    }else if (larger==0){//image 1 is larger

                        float mapped_x = x * ((float)bmih2.biWidth / bmih_large.biWidth);
                        float mapped_y = y * ((float)bmih2.biHeight / bmih_large.biHeight);
                        
                        BYTE b2 = get_color_bilinear(bmih2,dataimg2, mapped_x, mapped_y, bmih2.biWidth, bmih2.biHeight, 0);
                        BYTE g2 = get_color_bilinear(bmih2,dataimg2, mapped_x, mapped_y, bmih2.biWidth, bmih2.biHeight, 1);
                        BYTE r2 = get_color_bilinear(bmih2,dataimg2, mapped_x, mapped_y, bmih2.biWidth, bmih2.biHeight, 2);

                        float bf2 = (float) b2 / 255.0;
                        float gf2 = (float) g2 / 255.0;
                        float rf2 = (float) r2 / 255.0;

                        bfr=bf1*float_ratio+bf2*(1-float_ratio);
                        gfr=gf1*float_ratio+gf2*(1-float_ratio);
                        rfr=rf1*float_ratio+rf2*(1-float_ratio);

                        dataimg1[3*x+y*correctWidth] = (BYTE)(fminf(fmaxf(bfr * 255, 0), 255));
                        dataimg1[3*x+y*correctWidth + 1] = (BYTE)(fminf(fmaxf(gfr * 255, 0), 255));
                        dataimg1[3*x+y*correctWidth + 2] = (BYTE)(fminf(fmaxf(rfr * 255, 0), 255));
                    }
                }
            }  

            exit(0);
        }
        
    }

    while(wait(0)>0);
    printf("All child processes finished. Proceeding to write output file...\n");  

    // wait(0);
    // time_req = clock() - time_req;
    // printf("Processor time taken for forking: %f "
    //        "seconds\n",
    //        (float)time_req / CLOCKS_PER_SEC);


//WRITING TO FILES--------------------------------------------------
    FILE* fileOut=fopen(OutputFile.c_str(),"wb");
    if (larger == 1) {  // Image 2 is larger
        bmfh_large = bmfh2;
    } else {  // Image 1 is larger
        bmfh_large = bmfh1;
    }

printf("bfType: %X\n", bmfh_large.bfType);
printf("bfSize: %u\n", bmfh_large.bfSize);
printf("bfOffBits: %u\n", bmfh_large.bfOffBits);
printf("biWidth: %d, biHeight: %d\n", bmih_large.biWidth, bmih_large.biHeight);

    fwrite(&bmfh_large.bfType,sizeof(bmfh_large.bfType),1,fileOut);
    fwrite(&bmfh_large.bfSize,sizeof(bmfh_large.bfSize),1,fileOut);
    fwrite(&bmfh_large.bfReserved1,sizeof(bmfh_large.bfReserved1),1,fileOut);
    fwrite(&bmfh_large.bfReserved2,sizeof(bmfh_large.bfReserved2),1,fileOut);
    fwrite(&bmfh_large.bfOffBits,sizeof(bmfh_large.bfOffBits),1,fileOut);
    //reading second structs data
    fwrite(&bmih_large,sizeof(tagBITMAPINFOHEADER),1,fileOut);
    if (larger==1){//image 2 is larger and therefore is the first one
            fwrite(dataimg2,bmih_large.biSizeImage,1,fileOut);
    }else if (larger==0){//image 1 is larger
            fwrite(dataimg1,bmih_large.biSizeImage,1,fileOut);
    }
    fclose(fileOut);
    munmap(dataimg1, bmih1.biSizeImage);
    munmap(dataimg2, bmih1.biSizeImage);
}