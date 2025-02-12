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

int main(){
    int fd = shm_open("sharedmem", O_RDWR | O_CREAT, 0777);
    ftruncate(fd, 1000 * sizeof(char));
    
    // Map shared memory
    char *p = (char*)mmap(NULL, 1000 * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    // **Zero out shared memory**
    memset(p, 0, 1000 * sizeof(char));

    while(strcmp(p, "quit") != 0){
        read(STDIN_FILENO,p,1000);
    }
    
    munmap(p, 1000 * sizeof(char));
    close(fd);
    return 0;
}
