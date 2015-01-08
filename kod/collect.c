#include <stdlib.h>
#include <stdio.h>
#include "rootset.h"
#include "imalloc.h"
#include "priv_imalloc.h"

//Skapar en adressSpace av rymden start till end.
AddressSpace mkAdressSpace(RawPtr start, RawPtr end) {
  AddressSpace space = malloc(sizeof(addressspace));
  space->start = start;
  space->end   = end;
  return space;
}

//Hittar den chunk vars data innehåller ptr och anropar traverseChunk för den chunken
static void findChunk(void *ptr, pList L, pChunk C){
  //Sätter itteratorn på first
  L->current = L->first;
  //Söker igenom chunk-datat och markerar den chunk som pekaren pekar till med 1 och anropar traverseChunk.
  while(L->current){
	AddressSpace A = mkAdressSpace((RawPtr)(L->current->data),(RawPtr)(L->current->data+L->current->size)); 
    if(inAddressSpace(ptr, A)){
      if(L->current!=C){
	AddressSpace s = mkAdressSpace((RawPtr)(L+sizeof(list)),(RawPtr)(L+sizeof(list)+(L->tSize)));
	L->current->mark = 1;
	//Anropar traverseChunk för den hittade chunken.
	traverseChunk(L->current, s, L);
      }
      L->current = NULL;
    }
    else{
      L->current = L->current->next;
    }
  }
  //Sätter itteratorn på first
  L->current = L->first;
} 

//Letar efter pekare i C av storlek void *
void traverseChunk(pChunk C, AddressSpace s, pList L){
  void **temp = C->data;
  void **datamax = C->data+C->size;
  while(temp<datamax){
    if(inAddressSpace(*temp,s)){
	  //Letar upp chunken som temp pekar till
      findChunk(temp, L, C);
    }
    temp = temp+sizeof(char);
  }
}

//Sätter alla chunks mark-bit till 0
static int markZero(pList L){
  //Iteratorn nollställs
  L->current = L->first;
  int count = 0;
  while(L->current){
    if(!L->current->mark){
      count++;
    }
    else{
      //Chunken sätts som oanvänd
      L->current->mark = 0;
    }
    //L->current = L->current->next;
    L->current = L->current->next;
  }
  L->current = L->first;
  return count;
}

//Sätter mark-bit till 1 för C
static void mark(void *ptr, pChunk C){
      C->mark = 1; 
}

//Sätter mark-bit till 0 på samtliga oanvända chunks och returnerar 1 om minst en chunk har frigorts, annars 0.
unsigned int collect(Memory M){
  void *ptr = M;
  ptr = ptr - sizeof(list);
  //Räknar antalet oanvända chunks
  int count = markZero(TOLIST);
  TOLIST->current = TOLIST->first;
  
  while(TOLIST->current){
      traverseStack(mkAdressSpace(TOLIST->current->data, (TOLIST->current->data)+(TOLIST->current->size-1)), mark, TOLIST->current);
    TOLIST->current = TOLIST->current->next;
  }
  TOLIST->current = TOLIST->first;
  
  AddressSpace s = mkAdressSpace((RawPtr)(TOLIST+sizeof(list)),(RawPtr)(TOLIST+sizeof(list)+(TOLIST->tSize-1)));

  while(TOLIST->current){
    if((TOLIST->current->mark)==1){
      traverseChunk(TOLIST->current, s, TOLIST);
    }
    TOLIST->current = TOLIST->current->next;
  }
  TOLIST->current = TOLIST->first;
  
  while(TOLIST->current){
    if((TOLIST->current->mark)==0){
	count--;
    }
    TOLIST->current = TOLIST->current->next;
  }
  TOLIST->current = TOLIST->first;
    if(TOLIST->sort == 4)
    {
      memConcat(ptr);
    }
  if(count>=0){
    return 0;
  }
  return 1;
}
