#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <bits/stdc++.h>

#include <cmath>
#include <math.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
using namespace std;

int main2(){
    sleep(1);
    int fd= shm_open("sharedmem",O_RDWR,0777);
    if (fd!=-1){
        cout<<"sucessfully linked"<<endl;
    }
    char *p=(char*)mmap(NULL,1000*sizeof(char),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    //keeps checking memory
    while(p[0] != '\0'){
        cout<<p<<endl;
    }
}