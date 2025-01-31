#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <bits/stdc++.h>
#include <string>
#include <string.h>
#include <cmath>
#include <math.h>

using namespace std;
typedef unsigned char BYTE;

struct hmem {
    void *p, *n;
    int size;
    int occ;
};

void* head = NULL;

void *myMalloc(int size) {
    cout<<size<<endl;
    size += sizeof(hmem);
    cout<<size<<endl;
    //only have to change program break in the first and last case becasue
    //the middle case is only for cases where the program break needs to be created or moved
    // Initialize the head if it doesn't exist
    if (head == NULL) {
        int b = size / 4096;
        cout<<b<<endl;
        int mem = (b + 1) * 4096;
        cout<<mem<<endl;
        void *PB = sbrk(mem); 
        hmem* p = (hmem*) PB; 
        head = p;             // Set the global head pointer
        p->p = p->n = 0;   
        p->occ = 1;           
        p->size = mem;        
        cout<<"aerearaerarae"<<endl;
        return (void*) (p + 1); // Return pointer to the user memory
    } else {
        // Traverse the linked list to find a suitable blocks
        for (hmem* i = (hmem*) head; i != NULL; i = (hmem*) i->n) {
            // If block is unoccupied and large enough
            if (i->occ == 0 && i->size >= size) {
                i->occ = 1; // Mark as occupied
                return (void*) (i + 1); // Return pointer to user memory
            }
        }

        // If no suitable block found, extend the memory at the end
        hmem* i = (hmem*) head;
        while (i->n != NULL) { // Traverse to the last block
            i = (hmem*) i->n;
        }
        cout<<"here";
        // Allocate additional memory using sbrk
        int b = size / 4096;

        int mem = (b + 1) * 4096;
        void *PB = sbrk(mem);
        hmem* p = (hmem*) PB; 
        p->p = i;             
        p->n = NULL;          // No next block
        p->occ = 1;           // Mark as occupied
        p->size = mem;        // Set size of allocated memory
        i->n = p;             // Link previous block to new block
        return (void*) (p + 1); // Return pointer to user memory
        cout<<"here2";
    }
}


 
void *myFree(void* addr){
    //iterate throught the *linked list and look for the inputted adress
     hmem *chunk=(hmem*) addr-1;
    chunk->occ=0;
    if(chunk->n==NULL){
        if (chunk->p) {
            ((hmem*)chunk->p)->n = NULL;
        } else {
            head = NULL;
        }
        sbrk(-chunk->size);
        return;
    }else if(chunk->n && ((hmem*)chunk->n)->occ == 0){
        //create a next pointer which is the next of the chunk, add its size to the chunk, then set the pointer to the next of the next block
        //essentially just like adding the size to the original chunk adn getting rid of the pointer to the next one (rerouting it to the one after)
        hmem* nextBlock = (hmem*)chunk->n;
        chunk->size += nextBlock->size; 
        chunk->n = nextBlock->n; 
        if (nextBlock->n) {
            ((hmem*)nextBlock->n)->p = chunk;
        }
    }else if(chunk->p && ((hmem*)chunk->p)->occ == 0){
        //create a previous pointer that is just the prev or chunk, add the size of chunk to prev and move the pointer to skip over the current chunk
        hmem* prevBlock = (hmem*)chunk->p;
        prevBlock->size += chunk->size ; 
        prevBlock->n = chunk->n ; 
        if (prevBlock->p) {
            ((hmem*)prevBlock->p)->n = chunk; 
        }
    }else if(chunk->n && ((hmem*)chunk->n)->occ == 0 && chunk->p && ((hmem*)chunk->p)->occ == 0){
        //add the size to the previous block adn reference everything from there
        hmem* prevBlock = (hmem*)chunk->p;
        hmem* nextBlock = (hmem*)chunk->n;
        prevBlock->size += chunk->size + nextBlock->size; 
        prevBlock->n = nextBlock->n;
        if (nextBlock->n) {
            ((hmem*)nextBlock->n)->p = prevBlock; 
        }
    }else{
        cout<<"FREE DIDNT WORK"<<endl;
    }





    // first check adn see fi the adress does nto exist fjust return null 
    //ither wise if its found, als check if the occupoed variable is 1
    //fif the chunk is the last one than you can mover the progrram break back
    //then go through the list again to see if there are any adjacent chunks 
    //that are free adn merge them
    
}

int main(){
    int* data=(int*)myMalloc(30);
    cout<<data;
    int* data2=(int*)myMalloc(30);
    cout<<data2;
    // BYTE*a[100];
    // analyze();//50% points
    // for(int i=0;i<100;i++)
    // a[i]= myMalloc(1000);
    // for(int i=0;i<90;i++)
    // myFree(a[i]);
    // analyze(); //50% of points if this is correct
    // myFree(a[95]);
    // a[95] = myMalloc(1000);
    // analyze();//25% points, this new chunk should fill the smaller free one
    // //(best fit)
    // for(int i=90;i<100;i++)
    // myFree(a[i]);
    // analyze();// 25% should be an empty heap now with the start address
    // //from the program start
}


//you can change it when you aim for performance 

hmem* get_last_chunk() {
    if(!head) //I have a global void *startofheap = NULL;
    return NULL;
    hmem* ch = (hmem*)head;
    for (; ch->n; ch = (hmem*)ch->n);
    return ch;
}

void analyze(){
printf("\n--------------------------------------------------------------\n");
    if(!head)
    {
    printf("no heap");
    return;
    }
    hmem* ch = (hmem*)head;
    for (int no=0; ch; ch = (hmem*)ch->n,no++){
    printf("%d | current addr: %x |", no, ch);
    printf("size: %d | ", ch->size);
    printf("info: %d | ", ch->occ);
    printf("next: %x | ", ch->n);
    printf("prev: %x", ch->p);
    printf(" \n");
    }
    printf("program break on address: %x\n",sbrk(0));
}
//this means there is already a head 
    //loop through to see where there is space
        //thisis represended by a for loop
        //this means that firstly occupancy is 0 and secondly that there is enough space