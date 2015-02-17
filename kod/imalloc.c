#include <stdlib.h>
#include "rootset.h"
#include "imalloc.h"
#include "priv_imalloc.h"
#include <stdio.h>

pList globalpList;

void initChunk(int size, void *ptr, int caseSize){
  TOCHUNK->data = (ptr+caseSize);
  TOCHUNK->size = size;
  TOCHUNK->next = NULL;
  TOCHUNK->prev = NULL;
  TOCHUNK->mark = 0;
  TOCHUNK->refcount = 0;
}


void initList(int size, int sort, void *ptr){
  void *stepToChunk = (ptr-sizeof(chunk));
  TOLIST->first = stepToChunk;
  TOLIST->last = stepToChunk;
  TOLIST->current = stepToChunk;
  TOLIST->sort = sort;
  TOLIST->tSize = size;
  globalpList = TOLIST;
  }

void initManual(void *ptr){
  TOMANUAL->alloc = &allocInt;
  TOMANUAL->avail = &availableMemory;
  TOMANUAL->free = &freeMem;
  }

void initManaged(void *ptr, int select){
  TOMANAGED->alloc = &allocInt;
  if(select == 1){
  	TOMANAGED->rc.retain = &retain;
  	TOMANAGED->rc.release = &release;
  	TOMANAGED->rc.count = &count;
  }
  else if(select == 2){
	TOMANAGED->gc.alloc = &allocChar;
	TOMANAGED->gc.collect = &collect;
  }
  else{
  	TOMANAGED->rc.retain = &retain;
  	TOMANAGED->rc.release = &release;
  	TOMANAGED->rc.count = &count;
  	TOMANAGED->gc.alloc = &allocChar;
	TOMANAGED->gc.collect = &collect;
  }
}

struct style *iMalloc(chunk_size memsiz, unsigned int flags){  

  void *ptr;
  int caseSize = sizeof(chunk)+sizeof(list);
  switch(flags) {
  case 9: //manual + ascending size
    {
    caseSize = caseSize+sizeof(manual);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 1, ptr+sizeof(chunk));
    initManual(ptr+(sizeof(list)+sizeof(chunk)));
    ptr = ptr+caseSize-sizeof(manual);
    break;
  }
  case 10: //manual + descending size
    {
    caseSize = caseSize+sizeof(manual);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 2, ptr+sizeof(chunk));
    initManual(ptr+(sizeof(list)+sizeof(chunk)));
    ptr = ptr+caseSize-sizeof(manual);
    break;
    }
  case 12: //manual + adress
    {
    caseSize = caseSize+sizeof(manual);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 4, ptr+sizeof(chunk));
    initManual(ptr+(sizeof(list)+sizeof(chunk)));
    ptr = ptr+caseSize-sizeof(manual);
    break;
    }  
  case 17: //managed(refcount) + ascending size
    {
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 1, ptr+sizeof(chunk));
    initManaged(ptr+(sizeof(list)+sizeof(chunk)), 1);
    ptr = ptr+caseSize-sizeof(managed);
    break; 
    } 
  case 18: //managed(refcount) + descending size
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 2, ptr+sizeof(chunk));
    initManaged(ptr+(sizeof(list)+sizeof(chunk)), 1);
    ptr = ptr+caseSize-sizeof(managed);
    break;
  case 20: //managed(refcount) + adress
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 4, ptr+sizeof(chunk));
    initManaged(ptr+sizeof(list)+sizeof(chunk), 1);
    ptr = ptr+caseSize-sizeof(managed);
    break;
  case 33: //managed(GCD) + ascending size
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 1, ptr+sizeof(chunk));
    initManaged(ptr+sizeof(list)+sizeof(chunk), 2);
    ptr = ptr+caseSize-sizeof(managed);
    break;
  case 34: //managed(GCD) + descending size
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 2, ptr+sizeof(chunk));
    initManaged(ptr+sizeof(list)+sizeof(chunk), 2);
    ptr = ptr+caseSize-sizeof(managed);
    break;
  case 36: //managed(GCD) + adress
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 4, ptr+sizeof(chunk));
    initManaged(ptr+sizeof(list)+sizeof(chunk), 2);
    ptr = ptr+caseSize-sizeof(managed);
    break;
  case 49: //managed(GCD + refcount) + ascending size
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 1, ptr+sizeof(chunk));
    initManaged(ptr+sizeof(list)+sizeof(chunk), 4);
    ptr = ptr+caseSize-sizeof(managed);
    break;
  case 50: //managed(GCD + refcount) + descending size
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 2, ptr+sizeof(chunk));
    initManaged(ptr+sizeof(list)+sizeof(chunk), 4);
    ptr = ptr+caseSize-sizeof(managed);
    break;
  case 52: //managed(GCD + refcount) + adress
    caseSize = caseSize+sizeof(managed);
    ptr = malloc(caseSize+memsiz);
    initChunk(memsiz, ptr, caseSize);
    initList(memsiz, 4, ptr+sizeof(chunk));
    initManaged(ptr+sizeof(list)+sizeof(chunk), 4);
    ptr = ptr+caseSize-sizeof(managed);
    break;
    } 
  return ((Memory) (ptr));
}

void iMallocFree(Memory mem){
  pChunk temp1 = (void*)mem-sizeof(list)-sizeof(chunk);
  pChunk temp2;
  pList l = (void*)mem-sizeof(list);
  l->current = l->first;
  while(l->current){
    temp2 = l->current;
    l->current = l->current->next;
    if (temp1 != temp2){
      free(temp2);
    } 
  }
  free(temp1);
}
