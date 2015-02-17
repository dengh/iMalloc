#include "rootset.h"
#include "imalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include "priv_imalloc.h"


// En global variabel som pekar på listan av chunks
pList globalpList;


// Går igenom listan av chunks och hittar den chunk som har sama void* som indatat till findAChunk,
// d.v.s hittar den chunk som innehåller rätt metadata för den inskickade void-pekaren.
static pChunk findAChunk(void *data){
  pList L = globalpList;
  L->current = L->first;
  while(L->current){
    if(L->current->data != data){
      L->current = L->current->next;
    } else {
      return L->current;
    }
  }
  return NULL;
}

	
//Går igenom alla chunks i listan. Går igenom address space för varje chunk(det minne chunkens void* pekar på) och tittar
// i fall void* ptr, som skickas in, pekar in i chunkens address spacet.
static void chunkMem(void *ptr){
  Memory mem;
  pList L = globalpList;
  L->current = L->first;
  while(L->current){
  	if(L->current->mark != 0){
	  	AddressSpace s = mkAdressSpace(((RawPtr)(L->current->data)), ((RawPtr)(L->current->data+L->current->size-1)));
	    if(inAddressSpace(ptr, s)){
			release(mem, L->current->data);
	    }
  	}
  	L->current = L->current->next;
    
  }
  L->current = L->first;
}

// En strukt som bara innehåller variabeln refcount
typedef struct refCount {
  unsigned int refcount;
} refCount, *pRefCount;

// Hittar rätt chunk för void* som skickas in, tolkar pekaren till chunken som pRefCount(pekare till refCount) och ökar på refcount med 1.
// Att använda en lokal refcount struct gör att man lätt kan öka refcounten på flera olika typer av datastrukturer.
unsigned int retain(void* data){
  pRefCount temp = (pRefCount)findAChunk(data);
  temp->refcount++;
  return temp->refcount;
}


// Hittar rätt chunk för void* som skickas in, tolkar pekaren till chunken som pRefCount(pekare till refCount) och returnerar refcount.
// Att använda en lokal refcount struct gör att man lätt kan returnera refcounten på flera olika typer av datastrukturer.
unsigned int count(void* data){
  pRefCount temp =  (pRefCount)findAChunk(data);
  return temp->refcount;
}


// Hittar rätt chunk för void* som skickas in, tolkar pekaren till chunken som pRefCount(pekare till refCount) och minskar refcount med 1.
// I fall att refcount blir 0 så sätt mark = 0(fri utrymmet) sedan går release igenom vare address i chunken c:s void*,
// tolkar det som en void* och ser i fall den pekar in i någon annan chunk i listan som inte är fri. 
// I fall någon sådan finns, kör release på den chunken också. Det leder till att strukturer refererade av den som frias också minska i refcount.
// Att använda en lokal refcount struct gör att man lätt kan releasea refcounten på flera olika typer av datastrukturer
unsigned int release(Memory mem, void *data){
  pChunk c = findAChunk(data);
  if(c == NULL){
  	return 0;
  }
  pRefCount temp =  (pRefCount)c;
  temp->refcount--;
  
  if (temp->refcount == 0){
    ((pChunk)(temp))->mark = 0;
    
    void **temp = (void**)data;
    void **chunkSize = data+c->size;
    while(temp < chunkSize){
    	  chunkMem(*temp);
    	  temp = temp +1;
    }
    return 0;
  }
  return temp->refcount;
}
