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
#include <fcntl.h>
#define MATSIZE 3
using namespace std;


// input formatting: program name, program id, total number of processes 
// good explanation in the lab pdf
int* id_status= (int*)mmap(0,12,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
int* released= (int*)mmap(0,4,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
int* first= (int*)mmap(0,4,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);

void gather(int* shared,int id){
    if(!(*first)){
        *released=0;
    }
    shared[id]=1;
    while(!(*released)){
        if(id==0&&(shared[0]&&shared[1]&&shared[2])){
            //clearing shared CHANGE THIS TO BE DONE USING FOR LOOP
            shared[0]=0;
            shared[1]=0;
            shared[2]=0;
            //reinitialize so that we know that one has been released
            *first=0;
            //released all
            *released=1;
            break;
        }
    }
}

void work(int id,int* shared){
    for(int i=0;i<10;i++){
        gather(id_status,id);
        if(id==0){
            cout<<"----------"<<endl;
        }
        cout<<id<<endl;
    }
}
// if(fork()==0){
// work(0,id_status);
// return 0;

// }
// if(fork()==0){
// work(1,id_status);
// return 0;
// }
// work(2,id_status);
// wait(0);

int main(int argc,char* argv[]){
    //GATHER FUNCTION FLAGS 
    *released=0;
    *first=0;
    //changing inputs into the required types
    int prog_id = atoi(argv[1]);
    int p_count = atoi(argv[2]);
    int fd;
    int (*A)[MATSIZE];
    int (*B)[MATSIZE];
    int (*M)[MATSIZE];
    //if its the first program, create the shared mem else justr opent it
    if (prog_id==0){
        fd = shm_open("SHAREDMEM", O_CREAT | O_RDWR, 0777);
        ftruncate(fd, sizeof(int) * MATSIZE * MATSIZE * 3);
    } else {
        fd = shm_open("SHAREDMEM", O_RDWR, 0777);
    }
    //setting the pointers to certian parts of the mmap
    void *ptr = mmap(NULL, sizeof(int) * MATSIZE * MATSIZE * 3, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    A = (int (*)[MATSIZE])ptr;
    B = (int (*)[MATSIZE])(ptr + sizeof(int) * MATSIZE * MATSIZE);
    M = (int (*)[MATSIZE])(ptr + 2 * sizeof(int) * MATSIZE * MATSIZE);

    if (prog_id==0){
        for (int i = 0; i < MATSIZE; i++) {
            for (int j = 0; j < MATSIZE; j++) {
                A[i][j] = rand() % 10;
                B[i][j] = rand() % 10;
            }
        }
    }
    gather(id_status, prog_id);
    
}