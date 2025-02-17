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
pid_t pid;
pid_t pid_kid;
int larger;


int main(int argc,char* argv[]){
     clock_t time_req;
    time_req = clock();

    // string imageFileOne= argv[1];
    // string imageFileTwo= argv[2];
    // string ratio= argv[3];
    // string process_number= argv[4];
    // string OutputFile= argv[5];

    string imageFileOne= "wolf.bmp";
    string imageFileTwo= "lion.bmp";
    string ratio= "0.5";
    string process_number= "4";
    string OutputFile= "test.bmp";

    // if(sizeof(argv)!=5){
    //     cout<<"the following is the format:"<<endl;
    //     cout<<"[programname] [imagefile1] [imagefile2] [ratio] [process#] [outputfile]"<<endl;
        
    // }
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
    dataimg1=(BYTE*)mmap(0,bmfh1.bfSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    fread(dataimg1,bmfh1.bfSize,1,file1);
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
    dataimg2=(BYTE*)mmap(0,bmfh2.bfSize,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

    fread(dataimg2,bmfh2.bfSize,1,file2);

    fclose(file2);
//-------------------------------------------------------------------------------------------------------------------------------------
//should onkly have one corrected width
    // LONG correctWidth1=3*bmih1.biWidth+(((bmih1.biWidth)*3)%4);
    // LONG correctWidth2=3*bmih2.biWidth+(((bmih2.biWidth)*3)%4);


    //ALSO DECIDING WHICH IMAGE HAS THE HGHER RESOLUTION 
    if(bmih1.biHeight<bmih2.biHeight){
        bmih_large.biHeight=bmih2.biHeight;
        bmih_large.biWidth=bmih2.biWidth;
        int larger= 1;
    }else{
        bmih_large.biHeight=bmih1.biHeight;
        bmih_large.biWidth=bmih1.biWidth;
        int larger= 0;
    }
        LONG correctWidth=3*bmih_large.biWidth+(((bmih_large.biWidth)*3)%4);

    //create the correct number of forks by saying that you will keep creating forks while the process number is not reched
    //and the process is not the parent
    for (int x = 0; x <= stoi(process_number) && pid==0; x++) {
        // I THINK LINEAR INTERPOLATION GOES HERE
        pid_kid=fork();

        if (pid_kid < 0) {
        perror("Fork failed");
        exit(1);
        }   

        if(pid_kid==0){
            //create a kid and if we are in the kid
            LONG process_height=bmih_large.biHeight/stoi(process_number);
            int fork_bottom=x*process_height; 
            int fork_top=(x==stoi(process_number)-1)? bmih_large.biHeight : fork_bottom+ process_height;
            printf("process %d: Handling rows %ld to %ld\n",x,fork_bottom,fork_top);
            //iterates through the section of the photo
            
            for (int y = fork_bottom; y < fork_top; y++) {
                for (int x = 0; x < bmih_large.biWidth; x++) {
                    //getting the data from the photo and turning it into float form
                    BYTE b1=dataimg1[3*x+y*correctWidth];
                    BYTE g1=dataimg1[3*x+y*correctWidth+1];
                    BYTE r1=dataimg1[3*x+y*correctWidth+2];
                    float bf1=(float)b1/255;
                    float gf1=(float)g1/255;
                    float rf1=(float)r1/255;

                    BYTE b2=dataimg2[3*x+y*correctWidth];
                    BYTE g2=dataimg2[3*x+y*correctWidth+1];
                    BYTE r2=dataimg2[3*x+y*correctWidth+2];
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
                        bfr=bf2*float_ratio+bf1*(1-float_ratio);
                        gfr=gf2*float_ratio+gf1*(1-float_ratio);
                        rfr=rf2*float_ratio+rf1*(1-float_ratio);

                    dataimg2[3*x+y*correctWidth] = (BYTE)(fminf(fmaxf(bfr * 255, 0), 255));
                    dataimg2[3*x+y*correctWidth + 1] = (BYTE)(fminf(fmaxf(gfr * 255, 0), 255));
                    dataimg2[3*x+y*correctWidth + 2] = (BYTE)(fminf(fmaxf(rfr * 255, 0), 255));
                    }else if (larger==0){//image 1 is larger
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
    for (int i = 0; i < stoi(process_number); i++) {
        wait(0);  // OR NULL
    }
    printf("All child processes finished. Proceeding to write output file...\n");  

    // wait(0);
    // time_req = clock() - time_req;
    // printf("Processor time taken for forking: %f "
    //        "seconds\n",
    //        (float)time_req / CLOCKS_PER_SEC);


//WRITING TO FILES--------------------------------------------------
    FILE* fileOut=fopen(OutputFile.c_str(),"wb");
 //reading the first structs infomation (not mult of 4 data so we have to read one by one)
    fwrite(&bmfh_large.bfType,sizeof(bmfh_large.bfType),1,fileOut);
    fwrite(&bmfh_large.bfSize,sizeof(bmfh_large.bfSize),1,fileOut);
    fwrite(&bmfh_large.bfReserved1,sizeof(bmfh_large.bfReserved1),1,fileOut);
    fwrite(&bmfh_large.bfReserved2,sizeof(bmfh_large.bfReserved2),1,fileOut);
    fwrite(&bmfh_large.bfOffBits,sizeof(bmfh_large.bfOffBits),1,fileOut);
    //reading second structs data
    fwrite(&bmih_large,sizeof(tagBITMAPINFOHEADER),1,fileOut);
    if (larger==1){//image 2 is larger and therefore is the first one
            fwrite(dataimg2,bmfh_large.bfSize,1,fileOut);
    }else if (larger==0){//image 1 is larger
            fwrite(dataimg1,bmfh_large.bfSize,1,fileOut);
    }
    fclose(fileOut);
}