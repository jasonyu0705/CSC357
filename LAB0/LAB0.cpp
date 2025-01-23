#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;
struct llew;
struct lle{
    char c;
    struct lle *p=0;
    struct lle *n=0;
    struct llew *ew=0;
};

struct llew{
    char text[1000];
    llew *p=0;
    llew *n=0;
};

lle* lastone(lle *head){
lle *p=head;
for(;p->n!=0;p=p->n);
    return p;
}

llew* lastoneWord(llew *head){
llew *p=head;
for(;p->n!=0;p=p->n);
    return p;
}

int main(){
    lle *head=0;
    //adds things to letter array
    for(char ch='A';ch<='Z';ch++){
        lle *elem=(lle*)malloc(sizeof(lle));
        if(head==0){
            head=elem;
            head->c=ch;
        }else{
            lle *last=lastone(head);
            last->n= elem;
            elem->p=last;
            elem->c=ch;
        }
    }
    // getting user input
    char input[1000];
    cin >> input;
    char word[1000];
        for(int j=0;j<1000;j++){
        word[j]=0;
    }
    int count=0;
   
    //word[1000]= strtok(input,",");
    //iterate through each character in the array
    for(int i=0; input[i] - 1 != 0; i++){
        //if the word is not done and keep going until it is

      //  cout<<i<<endl;
        if(strcmp("PRINT",word)==0 ||strcmp("print",word)==0){
            //cout<<"entering else" << endl;
            //this means that the word does have a word in it
            //check if the word is print or not 

            //this means that the word does hav
                //DONT TAKE ANYNORE INPUTS AND PRINT
                //use nessted for loopo to iterate through the 
                //2d list only printing the words
                for(lle* p=head;p!=0;p=p->n){
                    if(p->ew!=0){
                        for(llew* pnt=p->ew;pnt!=0;pnt=pnt->n){
                            for (int i = 0; pnt -> text[i] != 0; i++) {
                                cout << pnt -> text[i];
                            }
                            cout << "\n";
                            
                        }
                    }

                }break;

          }else if (input[i]!=','){ 
            word[count]=input[i];
            count++;
        }else{
                //if its not, we can worry about making the linked list logic
                //for loop to iterate over alphabet linked list 
                for(lle* p=head;p!=0;p=p->n){
                    //if cnaracter is the same as the start of the word 
                    if(word[0]==p->c || word[0]-32==p->c){
                        llew *elem=(llew*)malloc(sizeof(llew));
                        if(p->ew==0){
                            //set the word as the head
                            p->ew = elem;
                            strcpy(elem -> text, word);
                            //point letter head to head of word LL

                        }else{
                            //this needs to be new word head
                            llew* lastWord=lastoneWord(p -> ew);
                            lastWord->n= elem;
                            elem->p=lastWord;
                            strcpy(elem->text,word);

                        }
                        //free(word);
                        for(int j=0;j<1000;j++){
                            word[j]=0;
                        }
                        count=0;
                        break;
                    }

                }  
            }
            
        }
        //find the last letter adn the find the last oword of that last letter and iterate through it
        //from last letter to first
        lle* prevLet=lastone(head);
        for(lle* p=prevLet;p!=0;p=p->p){
            p=prevLet;
            //if theres a word there
            if(prevLet->ew!=0){
                //find the last one of the word list adn iterate through word list backwards
                llew* pnt=lastoneWord(p->ew);
                llew* prevWord=pnt;
                for(;pnt!=0;pnt=pnt->p){
                    //here ur at the end of a word list, clear the weord content,set the prev adn then free the word
                    //initially set the previous to just the pointer
                    
                    for (int i = 0; prevWord -> text[i] != 0; i++) {
                        prevWord -> text[i]=0;
                    }
                    //change prev to an actual prev value
                    if(prevWord!=NULL){
                    prevWord=prevWord->p;
                    //free the weord ur on (or the next of the prev)
                    free(prevWord);
                    }else{
                        break;
                    }
                }
                
            }
            if(prevLet->p==NULL){
                free(head);
            }else{
                prevLet=prevLet->p;
                //free the letter itself
                free(prevLet->n);
            }
        }
    }
    