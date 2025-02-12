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
    sleep(1);
    int fd = shm_open("sharedmem", O_RDWR, 0777);
    if (fd != -1){
        cout << "Successfully linked" << endl;
    } 

    char *p = (char*)mmap(NULL, 1000 * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    // **Ensure memory is not already set to quit**
    if (strcmp(p, "quit") == 0) {
        memset(p, 0, 1000 * sizeof(char));  // Clear the quit flag
    }

    // Keep checking memory
    while (true) {
        if (p[0] != '\0') { // Only print non-empty messages            
            cout << p << endl;
            memset(p, 0, 1000 * sizeof(char)); // Clear memory after reading
            munmap(p, 1000 * sizeof(char));
            close(fd);
            return 0;
        }
    }
}
