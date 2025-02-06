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
hmem* smallest= NULL;
//only have to change program break in the first and last case becasue
    //the middle case is only for cases where the program break needs to be created or moved

void* myMalloc(int size) {
    //add the size of the heafer to the size
    size += sizeof(hmem);
    //if the head has not been created, create it and then add the nesessary infomation to it 
    if (head == NULL) {
        int b = size / 4096;
        int mem = (b + 1) * 4096;
        void *PB = sbrk(mem); 
        hmem* p = (hmem*) PB; 
        head = p;            
        p->p = p->n = 0;   
        p->occ = 1;           
        p->size = mem;        
        return (void*) (p + 1); 
    } else {//otherwise there is already a head and we need to either make new space for it or find it
        // Traverse the linked list to find a suitable blocks
        for (hmem* i = (hmem*) head; i != NULL; i = (hmem*) i->n) {
            // If block is unoccupied and large enough
            if (i->occ == 0 && i->size >= size) {
                //update it so that its the smallest, this will not only find a free space but also
                //find the smallest free space
                if(smallest == NULL || i->size < smallest->size ){
                        smallest=i;
                }
            }
        }
        //if you found the smallest then puit it there 
        if(smallest!=NULL){
            smallest->occ = 1; 
            return (void*) (smallest + 1); 
        }
        



        // If no suitable block found, extend the memory at the end
        hmem* i = (hmem*) head;
        while (i->n != NULL) { 
            i = (hmem*) i->n;
        }
        //cout<<"here"<<endl;

        int b = size / 4096;
        int mem = (b + 1) * 4096;
        void *PB = sbrk(mem);
        hmem* p = (hmem*) PB; 
        p->p = i;             
        p->n = NULL;          
        p->occ = 1;           
        p->size = mem;        
        i->n = p;            
        return (void*) (p + 1); 
    }
}


 
void myFree(void* addr){
    //iterate throught the *linked list and look for the inputted adress
    hmem *chunk=(hmem*) addr-1;
    chunk->occ=0;
    //cout<<chunk->n<<endl;
    //cout<<chunk->p<<endl;
    //cout<<((hmem*)chunk->n)->occ<<endl;

    //cout<<((hmem*)chunk->p)->occ<<endl;
 if(chunk->n && ((hmem*)chunk->n)->occ == 0){
        //create a next pointer which is the next of the chunk, add its size to the chunk, then set the pointer to the next of the next block
        //essentially just like adding the size to the original chunk adn getting rid of the pointer to the next one (rerouting it to the one after)
        hmem* nextBlock = (hmem*)chunk->n;
        chunk->size += nextBlock->size; 
        chunk->n = nextBlock->n; 
        ((hmem*)nextBlock->n)->p = chunk;
        //cout<<"case1"<<endl;

    }else if(chunk->p && ((hmem*)chunk->p)->occ == 0){
        //create a previous pointer that is just the prev or chunk, add the size of chunk to prev and move the pointer to skip over the current chunk
        hmem* prevBlock = (hmem*)chunk->p;
        prevBlock->size += chunk->size ; 
        prevBlock->n = chunk->n ; 
        if (chunk->n!=NULL){
            ((hmem*)chunk->n)->p = prevBlock; 
        }
    
        chunk=prevBlock;
       // cout<<"case2"<<endl;
        
    }else if ((chunk->n && ((hmem*)chunk->n)->occ == 0) &&
             (chunk->p && ((hmem*)chunk->p)->occ == 0)) {  
    //same story as above but clearing both 

          hmem* prevBlock = (hmem*)chunk->p;
        hmem* nextBlock = (hmem*)chunk->n;
        prevBlock->size += chunk->size + nextBlock->size; 
        prevBlock->n = nextBlock->n;
        ((hmem*)nextBlock->n)->p = prevBlock; 
        //cout<<"case3"<<endl;
        chunk=prevBlock;
    }
 if(chunk->n==NULL){
        //if the head is 
        //((hmem*)chunk->p)->n = NULL;
        if (chunk->p) {
            ((hmem*)chunk->p)->n = NULL;
        } else {
            head = NULL;
            
        }
        sbrk(-chunk->size);
        
    }
    // else{
    //     cout<<"FREE NOT WROKING"<<endl;
    // }


    // first check adn see fi the adress does nto exist fjust return null 
    //ither wise if its found, als check if the occupoed variable is 1
    //fif the chunk is the last one than you can mover the progrram break back
    //then go through the list again to see if there are any adjacent chunks 
    //that are free adn merge them
    
}
//you can change it when you aim for performance 

//----------------------------------------------------------------------------------------------
//GIVEN CODE
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
    printf("no heap,program break on address: %x\n",sbrk(0));
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

int main(){
// int possibe_sizes[] = { 1000, 6000, 20000, 30000 };
// srand(time(0));
// BYTE* a[100];
// clock_t ca, cb;
// ca = clock();
// for (int i = 0; i < 100; i++)
// {
// a[i] =(BYTE*) myMalloc(possibe_sizes[rand() % 4]);
// }
// int count = 0;
// for (int i = 0; i < 99; i++)
// {
// if (rand() % 2 == 0)
// {
// myFree(a[i]);
// a[i] = 0;
// count++;
// }
// if (count >= 50)
// break;
// }
// for (int i = 0; i < 100; i++)
// {
// if (a[i] == 0)
// {
// a[i] = (BYTE*)myMalloc(possibe_sizes[rand() % 4]);
// }
// }
// for (int i = 0; i < 100; i++)
// {
// myFree(a[i]);
// }
// cb = clock();
// analyze();
// printf("duration: %f\n", (double)(cb - ca) / CLOCKS_PER_SEC);

    void*a[100];
    analyze();//50% points
    for(int i=0;i<100;i++){  
        a[i]= myMalloc(1000);
        //cout<<i<<endl; 
    }
    //cout<< a <<endl;
    //cout<<"hello"<<endl;
    for(int i=0;i<90;i++)
    myFree(a[i]);
    //cout<<"hello2"<<endl;
    analyze(); //50% of points if this is correct
    myFree(a[95]);
    a[95] = myMalloc(1000);
    analyze();//25% points, this new chunk should fill the smaller free one
    //(best fit)
    for(int i=90;i<100;i++)
    myFree(a[i]);
    analyze();// 25% should be an empty heap now with the start address
    //from the program start
}


