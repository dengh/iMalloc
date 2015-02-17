#include "priv_imalloc.h"
#include <stdlib.h>
#include <stdio.h>

#define FALSE 0
#define TRUE 1

// Skapar en ny chunk newChunk vars data delvis baseras på chunken c och där size sätts till bytes.
static pChunk mkChunkFrom (pChunk c, unsigned int bytes)
{
  pChunk newChunk;

  newChunk = (pChunk) malloc(sizeof(struct chunk));
  newChunk->refcount = 0;					
  newChunk->data = c->data + bytes;
  newChunk->size = c->size - bytes;
  newChunk->next = NULL;
  newChunk->prev = NULL;
  newChunk->mark = 0;
  
  return newChunk;
} 

// Insättning av en chunk sortObj i en lista vars sortering på chunksen är av minskande storlek. 
static void descendingSort (pList list, pChunk currentChunk, pChunk sortObj) {
  if (sortObj->size >= currentChunk->size) 
    {
      sortObj->next = currentChunk;
      currentChunk->prev = sortObj;
      	list->first = sortObj;
    }
  else
    {
      while (currentChunk->next != NULL && sortObj->size < currentChunk->size)
	{
	  currentChunk = currentChunk->next;
	}
      if (sortObj->size < currentChunk->size)
	{
	  currentChunk->next = sortObj;
	  sortObj->prev = currentChunk;
	  list->last = sortObj;
	}
      else
	{
	  sortObj->prev = currentChunk->prev;
	  sortObj->prev->next = sortObj;
	  currentChunk->prev = sortObj;
	  sortObj->next = currentChunk;
	}
    }
}
      
// Insättning av en chunk sortObj i en lista vars sortering på chunksen är av ökad storlek. 
static void ascendingSort (pList list, pChunk currentChunk, pChunk sortObj) {
  if (sortObj->size <= currentChunk->size)
    {
      sortObj->next = currentChunk;
      currentChunk->prev = sortObj;
      list->first = sortObj;
    }
  else
    {
      while (currentChunk->next != NULL && sortObj->size > currentChunk->size)
	{
	  currentChunk = currentChunk->next;
	}
      if (sortObj->size > currentChunk->size)
	{
	  currentChunk->next = sortObj;
	  sortObj->prev = currentChunk;
	  list->last = sortObj;
	}
      else
	{
	  sortObj->prev = currentChunk->prev;
	  sortObj->prev->next = sortObj;
	  currentChunk->prev = sortObj;
	  sortObj->next = currentChunk;
	}
    }
}

// Kollar om data av storleken bytes ryms i chunken c. 
static int fits(pChunk c, unsigned int bytes) {
  return (c && c->size >= bytes && (c->mark == 0)) ? TRUE : FALSE;
}

// Pre: fits(c, bytes). 
// Sätter in data av storlek bytes i chunken c, och skapar en ny ledig, chunk,  
// om bytes är mindre än storleken av c och lägger denna efter c som c->next.   
static void *splitAddress(pList list, pChunk c, unsigned int bytes) {
  if (c->size > bytes) {
    pChunk temp = c->next;
    c->next = (pChunk) malloc(sizeof(struct chunk));
    c->next->refcount = 0;
    c->next->data = c->data + bytes;
    c->next->size = c->size - bytes;
    c->next->next = temp;
    if (temp) temp->prev = c->next;
    c->next->prev = c;
    c->next->mark = 0; 
    if (temp == NULL) list->last = c->next;  
  }
  c->refcount = 1;
  c->mark = 1; 
  c->size = bytes;  
  return (void*) c->data;
}
 
// Pre: fits(c, bytes). 
// Sätter in data av storlek bytes i chunken c, och gör en ny ledig, chunk,  
// om bytes är mindre än storleken av c och sorterar in denna i listan list
// på antingen stigande eller fallande storlek beroende på vilken sorterings-
// metod som angavs i anropet av iMalloc.  
static void *splitAscDesc(pList list, pChunk c, unsigned int bytes) 
{
  pChunk newChunk;
  if (c->size == bytes)
    {
      c->refcount = 1;
      c->mark = 1;
      c->size = bytes;
    }
  else if (c->size > bytes)
    {
      if (c->prev == NULL && c->next == NULL) 
	{
	  newChunk = mkChunkFrom(c, bytes);
	  
	  c->refcount = 1;
	  c->mark = 1;
	  c->size = bytes;
	  (list->sort == 1) ? ascendingSort(list, list->first, newChunk) : descendingSort(list, list->first, newChunk);
	}
      else if (c->prev == NULL)
	{
	  pChunk temp = c->next;

	  newChunk = mkChunkFrom(c, bytes);
	  
	  c->refcount = 1;
	  c->mark = 1;
	  c->size = bytes;
	  if (list->sort == 1)
	    {
	      ascendingSort(list, c, newChunk);	   
	    }
	  else
	    {
	      temp->prev = NULL;
	      c->prev = NULL; 
	      c->next = NULL;
	      list->first = temp;	  
	      descendingSort(list, list->first, newChunk);
	      descendingSort(list, list->first, c);
	    }
	}
      else if (c->next == NULL)
	{
	  newChunk = mkChunkFrom(c, bytes);
	  
	  c->refcount = 1;
	  c->mark = 1;
	  c->size = bytes;
	  c->prev->next = NULL;
	  c->prev = NULL; 
	  c->next = NULL; 
	  if (list->sort == 1)
	    {
	      ascendingSort(list, list->first, newChunk);
	      ascendingSort(list, list->first, c);
	    }
	  else
	    {
	      descendingSort(list, list->first, c);
	      descendingSort(list, list->first, newChunk);
	    }
	}
      else 
	{
	  newChunk = mkChunkFrom(c, bytes);
	  
	  c->refcount = 1;
	  c->mark = 1;
	  c->size = bytes;
	  c->prev->next = c->next;
	  c->next->prev = c->prev;
	  c->prev = NULL; 
	  c->next = NULL;
	  if (list->sort == 1)
	    {
	      ascendingSort(list, list->first, c);
	      ascendingSort(list, list->first, newChunk);
	    }
	  else
	    {
	      descendingSort(list, list->first, newChunk);
	      descendingSort(list, list->first, c);
	    }
	}
    }
  return (void *) c->data;
}

// Allokerar data av storlek bytes i minnesutrymmet mem och sorterar in detta
// enligt den sorteringsmetod som angavs vid anropet av iMalloc.
void *allocInt(Memory mem, chunk_size bytes) {
  void *ptr = mem;
  ptr = ptr-sizeof(list);

  pChunk c = TOLIST->first;
  
  if (bytes == 0) 
    {
      puts("Cannot allocate 0 bytes.\n");
      return NULL;
    }
  while (c && !fits(c, bytes)) c = c->next; 
  if (c) {
    return (TOLIST->sort == 4) ? splitAddress(ptr, c, bytes) : splitAscDesc(ptr, c, bytes); 
  } else {
    return NULL;
  }
}

// Allokerar data av storlek bytes, vars storlek anges av en formatsträng
// med något eller några av tecknena *, i, f, c, l och d, i minnesutrymmet 
// mem och sorterar in detta enligt den sorteringsmetod som angavs vid anropet av iMalloc.
void *allocChar(Memory mem, char* typeDesc) {
  int bytes = 0, i = 0;
  void *ptr = mem;
  ptr = ptr-sizeof(list);

  pChunk c = TOLIST->first;
  
  while (typeDesc[i] != '\0') {
    switch (typeDesc[i])
      {
      case '*': bytes = bytes + sizeof(void *); break;
      case 'i': bytes = bytes + sizeof(int); break;
      case 'f': bytes = bytes + sizeof(float); break;
      case 'c': bytes = bytes + sizeof(char); break;
      case 'l': bytes = bytes + sizeof(long); break;
      case 'd': bytes = bytes + sizeof(double); break;
      default: ;
      }
    i++;
  }
  if (bytes == 0) 
    {
      return NULL;
    }
  while (c && !fits(c, bytes)) c = c->next; 
  if (c) {
    return (TOLIST->sort == 4) ? splitAddress(ptr, c, bytes) : splitAscDesc(ptr, c, bytes); 
  } else {
    if(collect(mem)){
      return allocChar(mem, typeDesc);
    }
    return NULL;
  }
}

// Skriver ut alla chunks startadress, storlek och tillgänlighet i det aktuella minnesutrymmet.
void printMem(pList l) {
  pChunk c = l->first;
  
  while (c) {
    printf("Start address: %ld, Size: %d, Free: %d\n", (long) c->data, c->size, c->mark);
    c = c->next;
  }
  puts("---------------------------------------------");
}

// Returnerar det totala tillgängliga minnet i minnesutrymmet mem.
unsigned int availableMemory (Memory mem) {
  unsigned int space = 0;
  void *ptr = mem;
  ptr = ptr-sizeof(list);

  pChunk c = TOLIST->first;

  while(c)
    {
      if (c->mark == 0) space = space + c->size;
      c = c->next;
    }

  return space;
}

// Slår samman två bredvid varandra lediga chunks till en.
void memConcat(pList list) {
  pChunk c = list->first; 
  while(c && c->next) 
    {
      if (c->mark == 0 && c->next->mark == 0) 
	{
	  if (list->last == c->next) list->last = c;
	  pChunk temp = c->next;
	  c->size = c->size + temp->size; 
	  c->next = temp->next; 
	  free(temp);
	  c = list->first;
	}
      else
	{
	  c = c->next;
	}
    }
}

// Friar object ur minnesutrymmet mem och söker vid sorteringsmetoden ADDRESS(=4)
// igenom mem efter möjligheter att slå ihop lediga chunks med memConcat.
unsigned int freeMem (Memory mem, void *object) {
  void *ptr = mem;
  ptr = ptr-sizeof(list);

  pChunk temp = TOLIST->first;
  while(temp &&  temp->data != object) temp = temp->next;
  
  if (temp)
    {
      temp->mark = 0;
      
      if (TOLIST->sort == 4)
	{
	  memConcat(ptr);
	}
      return 1;
    }
  else return 0;
}
