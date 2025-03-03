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
int main(int argc,char* argv[]){
     clock_t time_req;
    time_req = clock();
    //initially take input and output  
    string imageFileOne= argv[1];
    string colourGradingR= argv[2];
    string colourGradingG= argv[3];
    string colourGradingB= argv[4];
    string OutputFile= argv[5];
    // string contrastFactor= argv[4];
    // cout<<imageFileOne<<endl;
    // cout<<OutputFile<<endl;
    // cout<<Operation<<endl;
    // cout<<contrastFactor<<endl;

    // string imageFileOne= "jar.bmp";
    // string colourGradingR= "0.2";
    // string colourGradingG= "0.2";
    // string colourGradingB= "0.2";
    // string OutputFile= "test1.bmp";


    //initialize the pointers to struct 
    tagBITMAPFILEHEADER bmfh;
    tagBITMAPINFOHEADER bmih;
    //actually reading in data
    BYTE* data;
    //new data for reading twice
    BYTE* data2;
    FILE* file=fopen(imageFileOne.c_str(),"rb");
    if (file==NULL){
        cout<<"NOT EXIST"<<endl;
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
    data=(BYTE*)mmap(0,bmfh.bfSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    data2=(BYTE*)mmap(0,bmfh.bfSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
    fread(data,bmfh.bfSize,1,file);
    //copying data so that we can read it again
    for (int i = 0; i < bmfh.bfSize; i++) {
        data2[i]=data[i];
    }
    fclose(file);
    LONG correctWidth=3*bmih.biWidth+(((bmih.biWidth)*3)%4);
    //FORKING
    if(fork()==0){
    //choose the opertion will each iterte throught 
    time_req = clock();
        for (int y = 0; y < bmih.biHeight/2; y++) {
            for (int x = 0; x < bmih.biWidth; x++) {
                //getting the data from the photo and turning it into float form
                int test=3*x+y*correctWidth;
                BYTE b=data[3*x+y*correctWidth];
                BYTE g=data[3*x+y*correctWidth+1];
                BYTE r=data[3*x+y*correctWidth+2];
                float bf=(float)b/255;
                float gf=(float)g/255;
                float rf=(float)r/255;

                //get the color grading for each colour to a float
                float cgr= atof(colourGradingR.c_str());
                float cgg= atof(colourGradingG.c_str());
                float cgb= atof(colourGradingB.c_str());
                //multiplying normalized value by correction factor from terminal
                bf*=cgb;
                gf*=cgg;
                rf*=cgr;
            //old lab code removed
                int bi=((int)(bf*255));
                if(bi>255){
                    bi=255;
                }else if(bi<0){
                    bi=0;
                }
                b=(BYTE)(bi);

                int gi=((int)(gf*255));
                if(gi>255){
                    gi=255;
                }else if(gi<0){
                    gi=0;
                }
                g=(BYTE)(gi);

                int ri=((int)(rf*255));
                if(ri>255){
                    ri=255;
                }else if(ri<0){
                    ri=0;
                }
                r=(BYTE)(ri);
                
                data[3*x+y*correctWidth]=b;
                data[3*x+y*correctWidth+1]=g;
                data[3*x+y*correctWidth+2]=r;
                //at this point the changed data is written to 
            }
        }      
return 0;
    
    }else{
    //choose the opertion will each iterte throught 
        for (int y = bmih.biHeight/2; y < bmih.biHeight; y++) {
            for (int x = 0; x < bmih.biWidth; x++) {
                //getting the data from the photo and turning it into float form
                int test=3*x+y*correctWidth;
                BYTE b=data[3*x+y*correctWidth];
                BYTE g=data[3*x+y*correctWidth+1];
                BYTE r=data[3*x+y*correctWidth+2];
                float bf=(float)b/255;
                float gf=(float)g/255;
                float rf=(float)r/255;

                //get the color grading for each colour to a float
                float cgr= atof(colourGradingR.c_str());
                float cgg= atof(colourGradingG.c_str());
                float cgb= atof(colourGradingB.c_str());
                //multiplying normalized value by correction factor from terminal
                bf*=cgb;
                gf*=cgg;
                rf*=cgr;
            //old lab code removed
                int bi=((int)(bf*255));
                if(bi>255){
                    bi=255;
                }else if(bi<0){
                    bi=0;
                }
                b=(BYTE)(bi);

                int gi=((int)(gf*255));
                if(gi>255){
                    gi=255;
                }else if(gi<0){
                    gi=0;
                }
                g=(BYTE)(gi);

                int ri=((int)(rf*255));
                if(ri>255){
                    ri=255;
                }else if(ri<0){
                    ri=0;
                }
                r=(BYTE)(ri);
                
                data[3*x+y*correctWidth]=b;
                data[3*x+y*correctWidth+1]=g;
                data[3*x+y*correctWidth+2]=r;
                //at this point the changed data is written to 
            }
        }      
    
    }
    wait(0);
    time_req = clock() - time_req;
    printf("Processor time taken for forking: %f "
           "seconds\n",
           (float)time_req / CLOCKS_PER_SEC);
     for (int y = 0; y < bmih.biHeight; y++) {
            for (int x = 0; x < bmih.biWidth; x++) {
                //getting the data from the photo and turning it into float form
                int test=3*x+y*correctWidth;
                BYTE b=data2[3*x+y*correctWidth];
                BYTE g=data2[3*x+y*correctWidth+1];
                BYTE r=data2[3*x+y*correctWidth+2];
                float bf=(float)b/255;
                float gf=(float)g/255;
                float rf=(float)r/255;

                //get the color grading for each colour to a float
                float cgr= atof(colourGradingR.c_str());
                float cgg= atof(colourGradingG.c_str());
                float cgb= atof(colourGradingB.c_str());
                //multiplying normalized value by correction factor from terminal
                bf*=cgb;
                gf*=cgg;
                rf*=cgr;
            //old lab code removed
                int bi=((int)(bf*255));
                if(bi>255){
                    bi=255;
                }else if(bi<0){
                    bi=0;
                }
                b=(BYTE)(bi);

                int gi=((int)(gf*255));
                if(gi>255){
                    gi=255;
                }else if(gi<0){
                    gi=0;
                }
                g=(BYTE)(gi);

                int ri=((int)(rf*255));
                if(ri>255){
                    ri=255;
                }else if(ri<0){
                    ri=0;
                }
                r=(BYTE)(ri);
                
                data2[3*x+y*correctWidth]=b;
                data2[3*x+y*correctWidth+1]=g;
                data2[3*x+y*correctWidth+2]=r;
                //at this point the changed data is written to 
            }
        }      
    wait(0);
    time_req = clock() - time_req;
    printf("Processor time taken without forking: %f "
           "seconds\n",
           (float)time_req / CLOCKS_PER_SEC);


    FILE* fileOut=fopen(OutputFile.c_str(),"wb");
 //reading the first structs infomation (not mult of 4 data so we have to read one by one)
    fwrite(&bmfh.bfType,sizeof(bmfh.bfType),1,fileOut);
    fwrite(&bmfh.bfSize,sizeof(bmfh.bfSize),1,fileOut);
    fwrite(&bmfh.bfReserved1,sizeof(bmfh.bfReserved1),1,fileOut);
    fwrite(&bmfh.bfReserved2,sizeof(bmfh.bfReserved2),1,fileOut);
    fwrite(&bmfh.bfOffBits,sizeof(bmfh.bfOffBits),1,fileOut);
    //reading second structs data
    fwrite(&bmih,sizeof(tagBITMAPINFOHEADER),1,fileOut);
    //WRITING DATA THIS MAY CHANGE WITH THE FORKING
    fwrite(data,bmfh.bfSize,1,fileOut);
    fclose(fileOut);
}
