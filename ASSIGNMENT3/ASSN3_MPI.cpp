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
pid_t pid;

//input formatting : program name, calculation program name, # processes 
int main(int argc,char* argv[]){
    //taking in inputs
    string calc_prog_name= argv[1];
    int num_processes= atoi(argv[2]);
    //pointers? ask about this
    char *args[4];
    args[0]=new char[100];//callee program name
    args[1]=new char[100];//prog_id
    args[2]=new char[100];//total program
    args[3]=0;//null terminator
    //formatting the progrtam name with./

    for (int i=0;i < num_processes;i++){
        strcpy(args[0],strcat("./",argv[1]));// callee program name
        sprintf(args[1],"%d",i);//prog_id
        sprintf(args[2],"%d",num_processes);//total processes
        pid=fork();
        if (pid < 0) {
            perror("fork failed");
            return 0;
        }

        if(pid==0){
            //args should be fine for syntax this may cause issue bc im unfarmiliar with this
            execv(args[0],args);
            return 0;
        }


    }
    while(wait(0)>0);
    return 0;
}





