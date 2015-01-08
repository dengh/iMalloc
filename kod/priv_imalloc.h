#ifndef __priv_malloc_h
#define __priv_malloc_h

#include "imalloc.h"

#define TOCHUNK ((pChunk)(ptr))
#define TOLIST ((pList)(ptr))
#define TOMANUAL ((Manual)(ptr))
#define TOMANAGED ((Managed)(ptr))


/*
 * priv_imalloc.h
 *
 * This file contains private specifications for using the imalloc
 * allocator used as a course project for the 2012 IOOP/M course.
 *
 * Note that this specification might evolve as the project is
 * running, f.ex. due to technical as well as educational bugs and
 * insights.
 *
 * You MAY ADD and ALTER the definitions in this file.
 *
 */


/* Actual return type specifications for iMalloc */

//Struct chunk:
//Strukturen chunk innehåller all metadata om det utrymme som användaren begär av funktionen alloc enligt följade:
//Refcount: Representerar hur många variabler som refererar chunk-datat. 
//Data: Pekar på det utrymme som användaren får tillbaka från anropet av alloc(och har rätt	att använda hur som helst).
//Size: Representerar storleken av data utrymmet, d.v.s. hur mycket utrymme som begärdes av alloc.
//Next: Pekar på nästa chunk i listan.
//Prev: Pekar på föregående chunk i listan. 
//Mark: Talar om i fall minnesutrymmet chunken beskriver är upptaged(1) eller ledigt(0). 
typedef struct chunk {
  unsigned int refcount;
  void *data;                    //pekare till datat i chunken
  chunk_size size;               //storleken på chunken
  struct chunk *next;                    //nästa objekt
  struct chunk *prev;                    //föregående objekt
  int   mark;                    //bool, ledigt eller ej  - 1 = upptaget, 0 = ledigt
}chunk, *pChunk;


//Stuct list:
//Strukturen list representerar en lista av chunks.
//First: Pekar på första chunken i listan.
//Current: Är en pekare som kan iterera över chunksen i listan.
//Last: Pekar på sista chunken i listan.
//Sort: Beskriver hur listan är sorterad(1=ascending, 2=descending, 4=address)
//tSize: Representerar totala utrymmet användaren allokerat med iMalloc.
typedef struct list {
  pChunk first;
  pChunk current;
  pChunk last;
  int    sort;                  
  int    tSize;                  
} list, *pList;


// En global variabel som pekar på listan av chunks
extern pList globalpList;


// Här ligger samtliga funktioner från modulerna för att dom ska kunna nås ifrån
// Imalloc.c och unittests.c.

//Här deklaresas samtliga funktioner ifrån F1.

// Allokerar data av storlek bytes i minnesutrymmet mem och sorterar in detta
// enligt den sorteringsmetod som angavs vid anropet av iMalloc.
void *allocInt(Memory mem, chunk_size bytes);

// Allokerar data av storlek bytes, vars storlek anges av en formatsträng
// med något eller några av tecknena *, i, f, c, l och d, i minnesutrymmet 
// mem och sorterar in detta enligt den sorteringsmetod som angavs vid anropet av iMalloc.
void *allocChar(Memory mem, char *bytes);

// Returnerar det totala tillgängliga minnet i minnesutrymmet mem.
unsigned int availableMemory (Memory mem);

// Friar object ur minnesutrymmet mem och söker vid sorteringsmetoden ADDRESS(=4)
// igenom mem efter möjligheter att slå ihop lediga chunks med memConcat.
unsigned int freeMem (Memory mem, void *object);

// Slår samman två bredvid varandra lediga chunks till en.
void memConcat(pList list);

//Här deklaresas samtliga funktioner ifrån F2.

// Hittar rätt chunk för void* som skickas in, tolkar pekaren till chunken som pRefCount(pekare till refCount) och ökar på refcount med 1.
// Att använda en lokal refcount struct gör att man lätt kan öka refcounten på flera olika typer av datastrukturer.
unsigned int retain(void* data);

// Hittar rätt chunk för void* som skickas in, tolkar pekaren till chunken som pRefCount(pekare till refCount) och returnerar refcount.
// Att använda en lokal refcount struct gör att man lätt kan returnera refcounten på flera olika typer av datastrukturer.
unsigned int count(void* data);

// Hittar rätt chunk för void* som skickas in, tolkar pekaren till chunken som pRefCount(pekare till refCount) och minskar refcount med 1.
// I fall att refcount blir 0 så sätt mark = 0(fri utrymmet) sedan går release igenom vare address i chunken c:s void*,
// tolkar det som en void* och ser i fall den pekar in i någon annan chunk i listan som inte är fri. 
// I fall någon sådan finns, kör release på den chunken också. Det leder till att strukturer refererade av den som frias också minska i refcount.
// Att använda en lokal refcount struct gör att man lätt kan releasea refcounten på flera olika typer av datastrukturer
unsigned int release(Memory mem,void* data);

//Här deklareras samtliga funktioner ifrån F3.

//Sätter mark-bit till 0 på samtliga oanvända chunks och returnerar 1 om minst en chunk har frigorts, annars 0.
unsigned int collect(Memory mem);


#endif
