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
#define MATSIZE 100
using namespace std;


// input formatting: program name, program id, total number of processes 
// good explanation in the lab pdf
int* id_status;

void print(long* A, long* B,long* M) {
    for (int i = 0; i < MATSIZE; i++) {
        for (int j = 0; j < MATSIZE; j++) {
            cout << A[i * MATSIZE + j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    
    for (int i = 0; i < MATSIZE; i++) {
        for (int j = 0; j < MATSIZE; j++) {
            cout << B[i * MATSIZE + j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    for (int i = 0; i < MATSIZE; i++) {
        for (int j = 0; j < MATSIZE; j++) {
            cout << M[i * MATSIZE + j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

//note:
//"ready" is a named shared array as big as par_count which needs to be initialized with zeros.
//Probably wait a second before forking the kids, so proc 0 can zero the "ready" array
void gather(int par_id, int par_count, int* ready)
{
    int synchid = ready[par_count] + 1;
    ready[par_id] = synchid;
    int breakout = 0;    
    while (1)
    {
        breakout = 1;
        for (int i = 0; i < par_count; i++)
        {
            if (ready[i] < synchid)
            {
                breakout = 0;
                break;
            }
        }
        if (breakout == 1)
        {
            ready[par_count] = synchid;
            break;
        }
    }
}

int main(int argc,char* argv[]){
    timespec start_time, end_time;
    //changing inputs into the required types
    int prog_id = atoi(argv[1]);
    int p_count = atoi(argv[2]);
    int ready;
    int Ashared;
    int Bshared;
    int Mshared;


    long *A;
    long *B;
    long *M;
    //if its the first program, create the shared mem else justr opent it

    if (prog_id==0){
        Ashared = shm_open("Ashared1", O_CREAT | O_RDWR, 0777);
        ftruncate(Ashared, sizeof(long) * MATSIZE * MATSIZE );
        Bshared = shm_open("Bshared1", O_CREAT | O_RDWR, 0777);
        ftruncate(Bshared, sizeof(long) * MATSIZE * MATSIZE );
        Mshared = shm_open("Mshared1", O_CREAT | O_RDWR, 0777);
        ftruncate(Mshared, sizeof(long) * MATSIZE * MATSIZE );
        ready = shm_open("readymem1", O_CREAT | O_RDWR, 0777);
        ftruncate(ready, sizeof(int) *(p_count+1));
    } else {
        sleep(1);
        Ashared = shm_open("Ashared1", O_RDWR, 0777);
        Bshared = shm_open("Bshared1", O_RDWR, 0777);
        Mshared = shm_open("Mshared1", O_RDWR, 0777);
        ready = shm_open("readymem1", O_RDWR, 0777);
    }
    //setting the pointers to certian parts of the mmap
    A = (long*)mmap(NULL, sizeof(long) * MATSIZE * MATSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, Ashared, 0);
    B = (long*)mmap(NULL, sizeof(long) * MATSIZE * MATSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, Bshared, 0);
    M = (long*)mmap(NULL, sizeof(long) * MATSIZE * MATSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, Mshared, 0);
    id_status= (int*)mmap(0,sizeof(int) *(p_count+1),PROT_READ|PROT_WRITE,MAP_SHARED,ready,0);

    //filling the matricies with random numbers
    if (prog_id==0){

        for (int i = 0; i < MATSIZE*MATSIZE; i++) {
                A[i] = rand() % 10;
                B[i] = rand() % 10;
                M[i]=0;
        }
        memset(id_status,0,sizeof(long) *(p_count+1));

    }else{
    sleep(1);
    }

    gather( prog_id,p_count,id_status);
    if (prog_id==0){
   
        clock_gettime(CLOCK_MONOTONIC, &start_time);
    }else{
        sleep(1);
    }

    int process_rows=MATSIZE/p_count;
    int extra_rows=MATSIZE%p_count;
    
    int start;
    int end;
    if (prog_id < extra_rows) {
        start = prog_id * (extra_rows + 1);
        end = start + process_rows + 1;
    } else {
        start = prog_id * process_rows + extra_rows;
        end = start + process_rows;
    }
    end = (end > MATSIZE) ? MATSIZE : end;

    for (int i = start; i < end; i++) {
        for (int j = 0; j < MATSIZE; j++) {
            M[i* MATSIZE+j] = 0;
            for (int k = 0; k < MATSIZE; k++) {
                M[i*MATSIZE+j] += A[i*MATSIZE+k] * B[k*MATSIZE+j];
            }
        }
    }
    gather( prog_id,p_count,id_status);
    if (prog_id==0){
       //print(A,B, M);
    }else{
        sleep(1);
    }
    

    for (int i = start; i < end; i++) {
        for (int j = 0; j < MATSIZE; j++) {
            B[i* MATSIZE+j] = 0;
            for (int k = 0; k < MATSIZE; k++) {
                B[i*MATSIZE+j] += M[i*MATSIZE+k] * A[k*MATSIZE+j];
            }
        }
    }
        gather( prog_id,p_count,id_status);
    if (prog_id==0){
       //print(A,B, M);
    }else{
        sleep(1);
    }
    for (int i = start; i < end; i++) {
        for (int j = 0; j < MATSIZE; j++) {
            M[i* MATSIZE+j] = 0;
            for (int k = 0; k < MATSIZE; k++) {
                M[i*MATSIZE+j] += B[i*MATSIZE+k] * A[k*MATSIZE+j];
            }
        }
    }
    gather( prog_id,p_count,id_status);

    if (prog_id==0){
       //print(A,B, M);
    }else{
        sleep(1);
    }

    if (prog_id==0){
        //calculate diagonal
        long diag = 0;
        for (int i = 0; i < MATSIZE; i++) {
            diag += M[i + i * MATSIZE];
        }
        cout<<"the sum of the diagonal of m is "<<diag<<endl;
        

        clock_gettime(CLOCK_MONOTONIC, &end_time);
            long time_passed = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_nsec - start_time.tv_nsec) / 1000;
            cout << "Time taken: " << time_passed << " us" << endl;
        shm_unlink("Ashared1");
        shm_unlink("Bshared1");
        shm_unlink("Cshared1");
        shm_unlink("readymem1");
    }else{
        sleep(1);
    }
    

    //clearing things
    munmap(id_status,sizeof(int)*p_count);
    munmap(A, sizeof(long) * MATSIZE * MATSIZE);
    munmap(B, sizeof(long) * MATSIZE * MATSIZE);
    munmap(M, sizeof(long) * MATSIZE * MATSIZE);
    close(Ashared);
    close(Bshared);
    close(Mshared);
    close(ready);
    return 0;
}