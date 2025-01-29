#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <cmath>
#include <math.h>
#include <unistd.h>

typedef unsigned char BYTE;

struct hmem {
    void *p, *n;
    int size;
    int occ;
};

void* head = NULL;

void *myMalloc(int size) {
    size += sizeof(hmem);
    //only have to change program break in the first and last case becasue
    //the middle case is only for cases where the program break needs to be created or moved
    // Initialize the head if it doesn't exist
    if (head == NULL) {
        int b = size / 4000;
        int mem = (b + 1) * 4000;
        void *PB = sbrk(mem); 
        hmem* p = (hmem*) PB; 
        head = p;             // Set the global head pointer
        p->p = p->n = 0;   
        p->occ = 1;           
        p->size = mem;        
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
        int b = size / 4000;
        int mem = (b + 1) * 4000;
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
    //





    // first check adn see fi the adress does nto exist fjust return null 
    //ither wise if its found, als check if the occupoed variable is 1
    //fif the chunk is the last one than you can mover the progrram break back
    //then go through the list again to see if there are any adjacent chunks 
    //that are free adn merge them
    
}

int main(){
    int* data=(int*)malloc(40);
    cout<<data;
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

// chunkhead* get_last_chunk() {
//     if(!head) //I have a global void *startofheap = NULL;
//     return NULL;
//     chunkhead* ch = (chunkhead*)startofheap;
//     for (; ch->next; ch = (chunkhead*)ch->next);
//     return ch;
// }

// void analyze(){
// printf("\n--------------------------------------------------------------\n");
//     if(!head)
//     {
//     printf("no heap");
//     return;
//     }
//     chunkhead* ch = (chunkhead*)startofheap;
//     for (int no=0; ch; ch = (chunkhead*)ch->next,no++){
//     printf("%d | current addr: %x |", no, ch);
//     printf("size: %d | ", ch->size);
//     printf("info: %d | ", ch->info);
//     printf("next: %x | ", ch->next);
//     printf("prev: %x", ch->prev);
//     printf(" \n");
//     }
//     printf("program break on address: %x\n",sbrk(0));
// }
//this means there is already a head 
    //loop through to see where there is space
        //thisis represended by a for loop
        //this means that firstly occupancy is 0 and secondly that there is enough space