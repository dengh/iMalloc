#include <stdlib.h>
#include <stdio.h>
#include "CUnit/Basic.h"
#include "imalloc.h"
#include "priv_imalloc.h"
#include "rootset.h"

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}


void testALLOC_ASCENDING_SIZE(void)
{
  Manual foo = (Manual)iMalloc(2 Mb, MANUAL + ASCENDING_SIZE);
  Memory bar = (Memory)foo;
  
  pList L = ((void*)foo)-sizeof(list);
  
  (CU_ASSERT(foo->avail(bar) == 2 Mb));
	
  // Test of one allocation on an empty iMalloc space
  void *a = foo->alloc(bar, 1 Mb + 512 Kb);
  (CU_ASSERT(foo->avail(bar) == 512 Kb));
  (CU_ASSERT(L->first->mark == 0));
  (CU_ASSERT(L->first->size == 512 Kb));
  (CU_ASSERT(L->first->next->mark == 1));
  (CU_ASSERT(L->first->next->size == 1 Mb + 512 Kb));
  (CU_ASSERT(a == L->last->data));
  (CU_ASSERT(L->last->mark == 1));
  (CU_ASSERT(L->last->size == 1 Mb + 512 Kb));
  (CU_ASSERT(L->first->size <= L->last->size));
  
  // Test of allocating data in the left-most free chunk
  void *b = foo->alloc(bar, 256 Kb);
  (CU_ASSERT(foo->avail(bar) == 256 Kb));
  (CU_ASSERT(L->first->mark == 0));
  (CU_ASSERT(L->first->size == 256 Kb));
  (CU_ASSERT(L->first->next->data == b));
  (CU_ASSERT(L->first->next->mark == 1));
  (CU_ASSERT(L->first->next->size == 256 Kb));
  (CU_ASSERT(L->first->size <= L->first->next->size));

  // Filling the memory to the maximum
  void *c = foo->alloc(bar, 256 Kb);
  (CU_ASSERT(L->first->mark == 1));
  (CU_ASSERT(L->first->size == 256 Kb));
  (CU_ASSERT(L->first->next->mark == 1));
  (CU_ASSERT(L->first->next->size == 256 Kb));
  (CU_ASSERT(L->first->data == c));
  (CU_ASSERT(L->first->size <= L->first->next->size));
  (CU_ASSERT(foo->avail(bar) == 0));

  // Trying to exceed maximum storage
  void *d = foo->alloc(bar, 1 Kb);
  (CU_ASSERT(d == NULL));
  (CU_ASSERT(foo->avail(bar) == 0));
  (CU_ASSERT(L->first->mark == 1));
  (CU_ASSERT(L->first->size == 256 Kb));
  (CU_ASSERT(L->first->next->mark == 1));
  (CU_ASSERT(L->first->next->size == 256 Kb));
	
  // Test of allocating data in-between two unavailable chunks
  freeMem(bar, b);
  (CU_ASSERT(foo->avail(bar) == 256 Kb));
  void * e = foo->alloc(bar, 128 Kb);
  (CU_ASSERT(foo->avail(bar) == 128 Kb));
  (CU_ASSERT(L->first->next->mark == 1));
  (CU_ASSERT(L->first->next->size == 128 Kb));
  (CU_ASSERT(L->first->next->data == e));
  (CU_ASSERT(L->first->mark == 0));
  (CU_ASSERT(L->first->size == 128 Kb));
  freeMem(bar, c);
  (CU_ASSERT(foo->avail(bar) == 128 Kb + 256 Kb));
  CU_ASSERT(L->first->next->next->mark == 0);
  CU_ASSERT(L->first->next->next->size == 256 Kb);
  void *f = foo->alloc(bar, 129 Kb);
  (CU_ASSERT(foo->avail(bar) == 384 Kb - 129 Kb));
  (CU_ASSERT(L->last->prev->size == 129 Kb));
  (CU_ASSERT(L->last->prev->mark == 1));
  (CU_ASSERT(L->last->prev->data == f));
  (CU_ASSERT(L->last->prev->size <= L->last->size));
  (CU_ASSERT(L->last->prev->prev->size <= L->last->prev->size));
	
  // Test of allocating data in the right-most free chunk
  freeMem(bar, a);
  (CU_ASSERT(foo->avail(bar) == 255 Kb + (1 Mb + 512 Kb)));
  (CU_ASSERT(L->last->mark == 0));
  (CU_ASSERT(L->last->size == (1 Mb + 512 Kb)));
  void *g = foo->alloc(bar, 550 Kb);
  (CU_ASSERT(foo->avail(bar) == (1 Mb + 767 Kb) - 550 Kb));
  (CU_ASSERT(L->last->mark == 0));
  (CU_ASSERT(L->last->size == (1 Mb + 512 Kb) - (550 Kb)));
  (CU_ASSERT(L->last->prev->mark == 1));
  (CU_ASSERT(L->last->prev->size == 550 Kb));
  (CU_ASSERT(L->last->prev->data == g));
  (CU_ASSERT(L->last->prev->size <= L->last->size));
  (CU_ASSERT(L->last->prev->prev->size <= L->last->prev->size));

  // Cleaning the memory from all allocated objects
  freeMem(bar, d); freeMem(bar, e); freeMem(bar, f); freeMem(bar, g);
  (CU_ASSERT(foo->avail(bar) == 2 Mb));
}

void testALLOC_DESCENDING_SIZE(void)
{
  Manual foo = (Manual)iMalloc(2 Mb, MANUAL + DESCENDING_SIZE);
  Memory bar = (Memory)foo;
  pList L = ((void*)foo)-sizeof(list);

  (CU_ASSERT(foo->avail(bar) == 2 Mb));
  
  // Test of one allocation on an empty iMalloc space
  void *a = foo->alloc(bar, 1 Mb + 512 Kb);
  CU_ASSERT(L->first->mark == 1);
  CU_ASSERT(L->first->size == 1 Mb + 512 Kb);
  CU_ASSERT(L->first->data == a);
  CU_ASSERT(L->first->next->mark == 0);
  CU_ASSERT(L->first->next->size == 512 Kb);
  CU_ASSERT(L->last->mark == 0);
  CU_ASSERT(L->last->size == 512 Kb);
  CU_ASSERT(foo->avail(bar) == 512 Kb);

  // Test of allocating data in the right-most free chunk
  void *b = foo->alloc(bar, 256 Kb);
  CU_ASSERT(L->last->mark == 1);
  CU_ASSERT(L->last->size == 256 Kb);
  CU_ASSERT(L->last->data == b);
  CU_ASSERT(L->last->prev->mark == 0);
  CU_ASSERT(L->last->prev->size == 256 Kb);
  CU_ASSERT(L->first->next->mark == 0);
  CU_ASSERT(L->first->next->size == 256 Kb);
  CU_ASSERT(foo->avail(bar) == 256 Kb); 
  CU_ASSERT(L->last->size >= L->last->prev->size);
  
  // Filling the memory to the maximum
  void *c = foo->alloc(bar, 256 Kb);
  CU_ASSERT(L->first->next->mark == 1);
  CU_ASSERT(L->first->next->size == 256 Kb);
  CU_ASSERT(foo->avail(bar) == 0); 
 
  // Trying to exceed maximum storage
  void *d = foo->alloc(bar, 1 Kb);
  CU_ASSERT(d == NULL);
  CU_ASSERT(L->first->mark == 1);
  CU_ASSERT(L->first->size == 1 Mb + 512 Kb);
  CU_ASSERT(L->first->next->mark == 1);
  CU_ASSERT(L->first->next->size == 256 Kb);
  CU_ASSERT(L->last->mark == 1);
  CU_ASSERT(L->last->size == 256 Kb);
  CU_ASSERT(foo->avail(bar) == 0 Kb); 

  // Test of allocating data in the left-most free chunk
  freeMem(bar, a);
  CU_ASSERT(foo->avail(bar) == 1 Mb + 512 Kb);
  void *e = foo->alloc(bar, 100 Kb);
  CU_ASSERT(L->first->size == (1 Mb + 512 Kb) - 100 Kb);
  CU_ASSERT(L->last->mark == 1);
  CU_ASSERT(L->last->size == 100 Kb);
  CU_ASSERT(L->last->data == e);
  CU_ASSERT(L->last->size <= L->last->prev->size);
  CU_ASSERT(foo->avail(bar) == 1 Mb + 412 Kb); 

  // Test of allocating data in-between two unavailable chunks
  freeMem(bar, c);
  CU_ASSERT(foo->avail(bar) == (1 Mb + 412 Kb) + 256 Kb);
  void *f = foo->alloc(bar, 1 Mb + 412 Kb);
  CU_ASSERT(L->first->data == f);
  CU_ASSERT(foo->avail(bar) == 256 Kb);
  void *g = foo->alloc(bar, 130 Kb);
  CU_ASSERT(L->last->prev->prev->data == g);
  CU_ASSERT(L->last->prev->prev->size == 130 Kb);
  CU_ASSERT(L->last->prev->prev->mark == 1);
  CU_ASSERT(L->last->prev->mark == 0);
  CU_ASSERT(L->last->prev->size == 126 Kb);
  CU_ASSERT(L->last->prev->prev->size <= L->first->size);
  CU_ASSERT(L->last->prev->prev->size >= L->last->prev->size);
  CU_ASSERT(foo->avail(bar) == 126 Kb); 

  // Cleaning the memory from all allocated objects
  freeMem(bar, g); freeMem(bar, f); freeMem(bar, e); freeMem(bar, b);
  CU_ASSERT(foo->avail(bar) == 2 Mb); 
}

void testALLOC_ADDRESS(void)
{
  Manual foo = (Manual)iMalloc(2 Mb, MANUAL + ADDRESS);
  Memory bar = (Memory)foo;
  pList L = ((void*)foo)-sizeof(list);
  
  (CU_ASSERT(foo->avail(bar) == 2 Mb));
  
  // Test of one allocation on an empty iMalloc space
  void *a = foo->alloc(bar, 1 Mb + 512 Kb);
  (CU_ASSERT(L->first->mark == 1));
  (CU_ASSERT(L->first->size == 1 Mb + 512 Kb));
  (CU_ASSERT(L->last->mark == 0));
  (CU_ASSERT(L->last->size == 512 Kb));
  (CU_ASSERT(a == L->first->data));
  (CU_ASSERT(foo->avail(bar) == 2 Mb - (1 Mb + 512 Kb)));

  // Test of merging two consecutive free chunks
  freeMem(bar, a);
  (CU_ASSERT(foo->avail(bar) == 2 Mb));
  (CU_ASSERT(L->first->mark == 0));
  (CU_ASSERT(L->last->mark == 0));

  // Test of allocating data in the left-most free chunk
  void *b = foo->alloc(bar, 1 Mb);
  (CU_ASSERT(foo->avail(bar) == 1 Mb));
  (CU_ASSERT(L->first->size == 1 Mb));
  (CU_ASSERT(L->first->mark == 1));
  (CU_ASSERT(b == L->first->data));
  (CU_ASSERT(L->last->size == 1 Mb));
  (CU_ASSERT(L->last->mark == 0));
  void *c = foo->alloc(bar, 1 Mb);
  (CU_ASSERT(foo->avail(bar) == 0 Mb));
  (CU_ASSERT(L->last->size == 1 Mb));
  (CU_ASSERT(L->last->mark == 1));
  (CU_ASSERT(c == L->last->data));
  freeMem(bar, b);
  (CU_ASSERT(foo->avail(bar) == 1 Mb));
  void *d = foo->alloc(bar, 224 Kb);
  (CU_ASSERT(foo->avail(bar) == (1 Mb - 224 Kb)));
  (CU_ASSERT(L->first->mark == 1));
  (CU_ASSERT(L->first->size == 224 Kb));
  (CU_ASSERT(L->first->next->size == (1 Mb - 224 Kb)));
  void *e = foo->alloc(bar, 800 Kb);
  (CU_ASSERT(foo->avail(bar) == 0 Mb));
  
  // Verifying that the user-created pointers are correct
  (CU_ASSERT(d == L->first->data));
  (CU_ASSERT(e == L->first->next->data));
  (CU_ASSERT(c == L->last->data));

  // Verifying that the chunks are sorted correctly
  (CU_ASSERT(L->first->data < L->first->next->data));
  (CU_ASSERT(L->first->next->data < L->last->data));

  // Trying to exceed maximum storage
  void *junk = foo->alloc(bar, 1 Kb);
  (CU_ASSERT(junk == NULL));
  (CU_ASSERT(foo->avail(bar) == 0 Mb));
  
  // Test of allocating data in the right-most free chunk
  freeMem(bar, c);
  (CU_ASSERT(foo->avail(bar) == 1 Mb));
  void *f = foo->alloc(bar, 512 Kb);
  (CU_ASSERT(foo->avail(bar) == 512 Kb));
  (CU_ASSERT(L->last->mark == 0));
  (CU_ASSERT(L->last->size == (1 Mb - 512 Kb)));
  (CU_ASSERT(L->last->prev->mark == 1));
  (CU_ASSERT(L->last->prev->size == 512 Kb));
  (CU_ASSERT(f == L->last->prev->data));
  void *g = foo->alloc(bar, 512 Kb);
  (CU_ASSERT(foo->avail(bar) == 0 Mb));

  // Test of allocating data in-between two unavailable chunks
  freeMem(bar, f);
  freeMem(bar, e);
  (CU_ASSERT(foo->avail(bar) == 1312 Kb));
  void *h = foo->alloc(bar, 1 Mb);
  (CU_ASSERT(foo->avail(bar) == (1312 Kb - 1 Mb)));
  (CU_ASSERT(h == L->last->prev->prev->data));
  (CU_ASSERT(L->last->prev->prev->mark == 1));
  (CU_ASSERT(L->last->prev->prev->size == 1 Mb));
  (CU_ASSERT(L->last->prev->mark == 0));
  
  // Cleaning the memory from all allocated objects
  freeMem(bar, c); freeMem(bar, d); freeMem(bar, g); freeMem(bar, h); 
  (CU_ASSERT(foo->avail(bar) == 2 Mb));
}

void testCOLLECT_ASCENDING_GC_ONE(void)
{
	//Skapar ett Managed-minne med storlek 10
	Managed foo = (Managed)iMalloc(10, GCD + ASCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 0);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}
void testCOLLECT_ASCENDING_GC_TWO(void)
{
	//Skapar ett Managed-minne med storlek 10
	Managed foo = (Managed)iMalloc(10, GCD + ASCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	foo->alloc(bar, 1);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 1);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}

void testCOLLECT_ASCENDING_GC_LOTS(void)
{
	//Skapar ett Managed-minne med storlek 1000b
	Managed foo = (Managed)iMalloc(10, GCD + ASCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//Skapar 10 chunks med storlek 100b
	int i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	//testar om 10 chunks har aktiveras.
	int mark = 0;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
	
	
	//testar om collect ändrar markbit på samtliga 10 chunks
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	i = 10;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
		
	//Sätter femte chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->next->next->next->next->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp5 = L->first->next->next->next->next->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	
	//Skapar pekare till de första 5 chunkens, testar om de sätts rätt 
	void *temp1 = L->first->data;
	void *temp2 = L->first->next->data;
	void *temp3 = L->first->next->next->data;
	void *temp4 = L->first->next->next->next->data;
	
	test = foo->gc.collect(bar); 
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->next->mark == 0);
	
	//Sätter alla mark-bits till 1 och ser om de 5 sista ändras till 0
	L->current = L->first;
	while(i<10){
		L->current->mark = 1;
		i++;
		L->current = L->current->next;
	}

	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->next->mark == 0);

	free(foo); 
}

void testCOLLECT_ADDRESS_GC_ONE(void)
{
	//Skapar ett Managed-minne med storlek 10
	Managed foo = (Managed)iMalloc(10, GCD + ADDRESS);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 0);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}
void testCOLLECT_ADDRESS_GC_TWO(void)
{
	//Skapar ett Managed-minne med storlek 10b
	Managed foo = (Managed)iMalloc(10, GCD + ADDRESS);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	foo->alloc(bar, 1);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 1);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}

void testCOLLECT_ADDRESS_GC_LOTS(void)
{
	//Skapar ett Managed-minne med storlek 1000b
	Managed foo = (Managed)iMalloc(10, GCD + ADDRESS);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//Skapar 10 chunks med storlek 100b
	int i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	//testar om 10 chunks har aktiveras.
	int mark = 0;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
	
	
	//testar om collect ändrar markbit på samtliga 10 chunks
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	
	L->current = L->first;
	while(L->current){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
		
	//Sätter femte chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}

	L->first->next->next->next->next->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->next == NULL);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	void *temp5 = L->first->next->next->next->next->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	i = 0;
	while(i<9){
		foo->alloc(bar, 1);
		i++;
	}	
	//Skapar pekare till de första 5 chunkens, testar om de sätts rätt 
	void *temp1 = L->first->data;
	void *temp2 = L->first->next->data;
	void *temp3 = L->first->next->next->data;
	void *temp4 = L->first->next->next->next->data;
	
	test = foo->gc.collect(bar); 
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	
	//Sätter alla mark-bits till 1 och ser om de 5 sista ändras till 0
	i = 0;
	L->current = L->first;
	while(i<6){
		L->current->mark = 1;
		i++;
		L->current = L->current->next;
	}

	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	free(foo); 
}



void testCOLLECT_DESCENDING_GC_ONE(void)
{
	//Skapar ett Managed-minne med storlek 10
	Managed foo = (Managed)iMalloc(10, GCD + DESCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 0);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}
void testCOLLECT_DESCENDING_GC_TWO(void)
{
	//Skapar ett Managed-minne med storlek 10b
	Managed foo = (Managed)iMalloc(10, GCD + DESCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	foo->alloc(bar, 1);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 0);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}

void testCOLLECT_DESCENDING_GC_LOTS(void)
{
	//Skapar ett Managed-minne med storlek 10b
	Managed foo = (Managed)iMalloc(10, GCD + DESCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//Skapar 10 chunks med storlek 100b
	int i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	//testar om 10 chunks har aktiveras.
	int mark = 0;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
	
	
	//testar om collect ändrar markbit på samtliga 10 chunks
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	i = 10;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
		
	//Sätter femte chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->next->next->next->next->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp5 = L->first->next->next->next->next->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	
	//Skapar pekare till de första 5 chunkens, testar om de sätts rätt 
	void *temp1 = L->first->data;
	void *temp2 = L->first->next->data;
	void *temp3 = L->first->next->next->data;
	void *temp4 = L->first->next->next->next->data;
	
	test = foo->gc.collect(bar); 
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->next->mark == 0);
	
	//Sätter alla mark-bits till 1 och ser om de 5 sista ändras till 0
	L->current = L->first;
	while(i<10){
		L->current->mark = 1;
		i++;
		L->current = L->current->next;
	}

	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->next->mark == 0);

	free(foo); 
}

void testCOLLECT_ASCENDING_GC_RC_ONE(void)
{
	//Skapar ett Managed-minne med storlek 10
	Managed foo = (Managed)iMalloc(10, GCD + REFCOUNT + ASCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 0);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}
void testCOLLECT_ASCENDING_GC_RC_TWO(void)
{
	//Skapar ett Managed-minne med storlek 10b
	Managed foo = (Managed)iMalloc(10, GCD + REFCOUNT + ASCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	foo->alloc(bar, 1);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 1);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}

void testCOLLECT_ASCENDING_GC_RC_LOTS(void)
{
	//Skapar ett Managed-minne med storlek 10b
	Managed foo = (Managed)iMalloc(10, GCD + REFCOUNT + ASCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//Skapar 10 chunks med storlek 100b
	int i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	//testar om 10 chunks har aktiveras.
	int mark = 0;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
	
	
	//testar om collect ändrar markbit på samtliga 10 chunks
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	i = 10;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
		
	//Sätter femte chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->next->next->next->next->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp5 = L->first->next->next->next->next->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	
	//Skapar pekare till de första 5 chunkens, testar om de sätts rätt 
	void *temp1 = L->first->data;
	void *temp2 = L->first->next->data;
	void *temp3 = L->first->next->next->data;
	void *temp4 = L->first->next->next->next->data;
	
	test = foo->gc.collect(bar); 
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->next->mark == 0);
	
	//Sätter alla mark-bits till 1 och ser om de 5 sista ändras till 0
	L->current = L->first;
	while(i<10){
		L->current->mark = 1;
		i++;
		L->current = L->current->next;
	}

	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->next->mark == 0);

	free(foo); 
}


void testCOLLECT_ADDRESS_GC_RC_ONE(void)
{
	//Skapar ett Managed-minne med storlek 10
	Managed foo = (Managed)iMalloc(10, GCD + REFCOUNT + ADDRESS);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 0);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}
void testCOLLECT_ADDRESS_GC_RC_TWO(void)
{
	//Skapar ett Managed-minne med storlek 10b
	Managed foo = (Managed)iMalloc(1 Kb, GCD + REFCOUNT + ADDRESS);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	foo->alloc(bar, 1);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 1);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}

void testCOLLECT_ADDRESS_GC_RC_LOTS(void)
{
	//Skapar ett Managed-minne med storlek 1000b
	Managed foo = (Managed)iMalloc(10, GCD + ADDRESS);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//Skapar 10 chunks med storlek 100b
	int i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	//testar om 10 chunks har aktiveras.
	int mark = 0;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
	
	
	//testar om collect ändrar markbit på samtliga 10 chunks
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	
	L->current = L->first;
	while(L->current){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
		
	//Sätter femte chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}

	L->first->next->next->next->next->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->next == NULL);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	void *temp5 = L->first->next->next->next->next->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	i = 0;
	while(i<9){
		foo->alloc(bar, 1);
		i++;
	}	
	//Skapar pekare till de första 5 chunkens, testar om de sätts rätt 
	void *temp1 = L->first->data;
	void *temp2 = L->first->next->data;
	void *temp3 = L->first->next->next->data;
	void *temp4 = L->first->next->next->next->data;
	
	test = foo->gc.collect(bar); 
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	
	//Sätter alla mark-bits till 1 och ser om de 5 sista ändras till 0
	i = 0;
	L->current = L->first;
	while(i<6){
		L->current->mark = 1;
		i++;
		L->current = L->current->next;
	}

	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	free(foo); 
}

void testCOLLECT_DESCENDING_GC_RC_ONE(void)
{
	//Skapar ett Managed-minne med storlek 10
	Managed foo = (Managed)iMalloc(10, GCD + REFCOUNT + DESCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 0);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}
void testCOLLECT_DESCENDING_GC_RC_TWO(void)
{
	//Skapar ett Managed-minne med storlek 10b
	Managed foo = (Managed)iMalloc(10, GCD + REFCOUNT + DESCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	foo->alloc(bar, 1);
	
	//testar om första chunken initsieras korrekt.
	CU_ASSERT(L->first->mark == 0);
	
	//testar om markbitten behålls som 0 vid körning av collect och att collect returnerar 0
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);
	
	//Sätter chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp = L->first->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	free(foo);
}

void testCOLLECT_DESCENDING_GC_RC_LOTS(void)
{
	//Skapar ett Managed-minne med storlek 10b
	Managed foo = (Managed)iMalloc(10, GCD + REFCOUNT + DESCENDING_SIZE);
	//Skapar bar som är en Memory pekarare
	Memory bar = (Memory)foo;
	//Skapar L som är pekaren i listan som nås med pekararemetik
	pList L = ((void*)foo)-sizeof(list);
	
	//Skapar 10 chunks med storlek 1b
	int i = 0;
	while(i<10){
		foo->alloc(bar, 1);
		i++;
	}
	//testar om 10 chunks har aktiveras.
	int mark = 0;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
	
	
	//testar om collect ändrar markbit på samtliga 10 chunks
	int test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	i = 10;
	L->current = L->first;
	while(i>0){
		mark  = mark + L->current->mark;
		i--;
		L->current = L->current->next;
	}
	CU_ASSERT(mark == 10);
		
	//Sätter femte chunkens mark-bit till 1 för att se om den ställs om till 0 av collect
	L->first->next->next->next->next->mark = 1;
	test = foo->gc.collect(bar);
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 0);

	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	void *temp5 = L->first->next->next->next->next->data;
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	//Skapar en pekare på chunkens data och testar om collect gör som den skall
	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	
	//Skapar pekare till de första 5 chunkens, testar om de sätts rätt 
	void *temp1 = L->first->data;
	void *temp2 = L->first->next->data;
	void *temp3 = L->first->next->next->data;
	void *temp4 = L->first->next->next->next->data;
	
	test = foo->gc.collect(bar); 
	
	CU_ASSERT(test == 0);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->next->mark == 0);
	
	//Sätter alla mark-bits till 1 och ser om de 5 sista ändras till 0
	L->current = L->first;
	while(i<10){
		L->current->mark = 1;
		i++;
		L->current = L->current->next;
	}

	test = foo->gc.collect(bar);
	
	CU_ASSERT(test == 1);
	CU_ASSERT(L->first->mark == 1);
	CU_ASSERT(L->first->next->mark == 1);
	CU_ASSERT(L->first->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->mark == 1);
	CU_ASSERT(L->first->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->mark == 0);
	CU_ASSERT(L->first->next->next->next->next->next->next->next->next->next->mark == 0);

	free(foo); 
}

void testRC_ASCENDING(void)
{
	Managed foo = (Managed)iMalloc(2 Mb, REFCOUNT + ASCENDING_SIZE);
	Memory bar = (Memory)foo;
	pList L = ((pList)((void*)foo-sizeof(list)));
	
	void *ptr = foo->alloc(bar, 512 Kb);
	CU_ASSERT(foo->rc.count(ptr) == 1);
	CU_ASSERT(foo->rc.retain(ptr) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr) == 1);
		
	void *ptr2 = foo->alloc(bar, 1 Mb);
	CU_ASSERT(foo->rc.count(ptr2) == 1);
	CU_ASSERT(foo->rc.retain(ptr2) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr2) == 1);
	*((void**)(ptr)) = ptr2;

	
	void *ptr3 = foo->alloc(bar, 128 Kb);
	CU_ASSERT(foo->rc.count(ptr3) == 1);
	CU_ASSERT(foo->rc.retain(ptr3) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr3) == 1);
	*((void**)(ptr2)) = ptr3;
	
	CU_ASSERT(foo->rc.release(bar, ptr) == 0);
	L->current = L->first;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
}

void testRC_DESCENDING(void)
{
	Managed foo = (Managed)iMalloc(2 Mb, REFCOUNT + DESCENDING_SIZE);
	Memory bar = (Memory)foo;
	pList L = ((pList)((void*)foo-sizeof(list)));
	
	void *ptr = foo->alloc(bar, 1 Mb);
	CU_ASSERT(foo->rc.count(ptr) == 1);
	CU_ASSERT(foo->rc.retain(ptr) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr) == 1);

	
	void *ptr2 = foo->alloc(bar, 500 Kb);
	CU_ASSERT(foo->rc.count(ptr2) == 1);
	CU_ASSERT(foo->rc.retain(ptr2) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr2) == 1);
	*((void**)(ptr)) = ptr2;
	//printMem(L);

	
	void *ptr3 = foo->alloc(bar, 128 Kb);
	CU_ASSERT(foo->rc.count(ptr3) == 1);
	CU_ASSERT(foo->rc.retain(ptr3) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr3) == 1);
	*((void**)(ptr2)) = ptr3;
	
	CU_ASSERT(foo->rc.release(bar, ptr) == 0);
	L->current = L->first;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
}

void testRC_ADDRESS(void)
{
	Managed foo = (Managed)iMalloc(2 Mb, REFCOUNT + ADDRESS);
	Memory bar = (Memory)foo;
	pList L = ((pList)((void*)foo-sizeof(list)));
	
	void *ptr = foo->alloc(bar, 128 Kb);
	CU_ASSERT(foo->rc.count(ptr) == 1);
	CU_ASSERT(foo->rc.retain(ptr) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr) == 1);
	
	void *ptr2 = foo->alloc(bar, 1 Mb);
	CU_ASSERT(foo->rc.count(ptr2) == 1);
	CU_ASSERT(foo->rc.retain(ptr2) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr2) == 1);
	*((void**)(ptr)) = ptr2;

	
	void *ptr3 = foo->alloc(bar, 256 Kb);
	CU_ASSERT(foo->rc.count(ptr3) == 1);
	CU_ASSERT(foo->rc.retain(ptr3) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr3) == 1);
	*((void**)(ptr2)) = ptr3;
	
	CU_ASSERT(foo->rc.release(bar, ptr) == 0);
	L->current = L->first;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
}

void testRC_GC_ASCENDING(void)
{
	Managed foo = (Managed)iMalloc(2 Mb, GCD + REFCOUNT + ASCENDING_SIZE);
	Memory bar = (Memory)foo;
	pList L = ((pList)((void*)foo-sizeof(list)));
	
	void *ptr = foo->alloc(bar, 512 Kb);
	CU_ASSERT(foo->rc.count(ptr) == 1);
	CU_ASSERT(foo->rc.retain(ptr) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr) == 1);
		
	void *ptr2 = foo->alloc(bar, 1 Mb);
	CU_ASSERT(foo->rc.count(ptr2) == 1);
	CU_ASSERT(foo->rc.retain(ptr2) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr2) == 1);
	*((void**)(ptr)) = ptr2;

	
	void *ptr3 = foo->alloc(bar, 128 Kb);
	CU_ASSERT(foo->rc.count(ptr3) == 1);
	CU_ASSERT(foo->rc.retain(ptr3) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr3) == 1);
	*((void**)(ptr2)) = ptr3;
	
	CU_ASSERT(foo->rc.release(bar, ptr) == 0);
	L->current = L->first;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
}

void testRC_GC_DESCENDING(void)
{
	Managed foo = (Managed)iMalloc(2 Mb, GCD + REFCOUNT + DESCENDING_SIZE);
	Memory bar = (Memory)foo;
	pList L = ((pList)((void*)foo-sizeof(list)));
	
	void *ptr = foo->alloc(bar, 1 Mb);
	CU_ASSERT(foo->rc.count(ptr) == 1);
	CU_ASSERT(foo->rc.retain(ptr) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr) == 1);
	
	void *ptr2 = foo->alloc(bar, 128 Kb);
	CU_ASSERT(foo->rc.count(ptr2) == 1);
	CU_ASSERT(foo->rc.retain(ptr2) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr2) == 1);
	*((void**)(ptr)) = ptr2;

	
	void *ptr3 = foo->alloc(bar, 512 Kb);
	CU_ASSERT(foo->rc.count(ptr3) == 1);
	CU_ASSERT(foo->rc.retain(ptr3) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr3) == 1);
	*((void**)(ptr2)) = ptr3;
	
	CU_ASSERT(foo->rc.release(bar, ptr) == 0);
	L->current = L->first;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
}

void testRC_GC_ADDRESS(void)
{
	Managed foo = (Managed)iMalloc(2 Mb, GCD + REFCOUNT + ADDRESS);
	Memory bar = (Memory)foo;
	pList L = ((pList)((void*)foo-sizeof(list)));
	
	void *ptr = foo->alloc(bar, 128 Kb);
	CU_ASSERT(foo->rc.count(ptr) == 1);
	CU_ASSERT(foo->rc.retain(ptr) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr) == 1);
	
	void *ptr2 = foo->alloc(bar, 1 Mb);
	CU_ASSERT(foo->rc.count(ptr2) == 1);
	CU_ASSERT(foo->rc.retain(ptr2) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr2) == 1);
	*((void**)(ptr)) = ptr2;

	
	void *ptr3 = foo->alloc(bar, 256 Kb);
	CU_ASSERT(foo->rc.count(ptr3) == 1);
	CU_ASSERT(foo->rc.retain(ptr3) == 2);
	CU_ASSERT(foo->rc.release(bar, ptr3) == 1);
	*((void**)(ptr2)) = ptr3;
	
	CU_ASSERT(foo->rc.release(bar, ptr) == 0);
	L->current = L->first;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
	L->current = L->current->next;
	CU_ASSERT(L->current->mark == 0);
}

void testALLOCCHAR (void) 
{
  // Testing three sets of valid format strings
  Managed foo = (Managed)iMalloc(2 Mb, GCD + ADDRESS);
  Memory bar = (Memory)foo;
  pList L = ((pList)((void*)foo-sizeof(list)));
  
  void *a = foo->gc.alloc(bar, "***d");
  (CU_ASSERT(L->first->size == sizeof(void*)*3 + sizeof(double)));
  void *b = foo->gc.alloc(bar, "iicffd");
  (CU_ASSERT(L->first->next->size == sizeof(int)*2 + sizeof(char) + sizeof(float)*2 + sizeof(double)));
  void *c = foo->gc.alloc(bar, "llicfddfdc*d");
  (CU_ASSERT(L->first->next->next->size == sizeof(long)*2 + sizeof(int) + sizeof(char)*2 + sizeof(float)*2 + sizeof(double)*4 + sizeof(void*)));

  // Cleaning the memory from allocated objects
  freeMem(bar, a); freeMem(bar, b); freeMem(bar, c);


  // Testing an invalid format string
  Managed foo2 = (Managed)iMalloc(1 Mb, GCD + ADDRESS);
  Memory bar2 = (Memory)foo2;
  pList L2 = ((pList)((void*)foo2-sizeof(list)));
  
  void *d = foo2->gc.alloc(bar2, "Ros-Otto");
  (CU_ASSERT(L2->first->size == 1 Mb));
  (CU_ASSERT(L2->first->prev == NULL));
  (CU_ASSERT(L2->first->next == NULL));

  void *e = foo2->gc.alloc(bar2, "Rukkoa");
  (CU_ASSERT(L2->first->size == 1 Mb));
  (CU_ASSERT(L2->first->prev == NULL));
  (CU_ASSERT(L2->first->next == NULL));
  
  // Cleaning the memory from allocated objects  
  freeMem(bar2, d); freeMem(bar2, e);


  // Testing a string of both valid and invalid characters
  Managed foo3 = (Managed)iMalloc(1 Mb, GCD + ADDRESS);
  Memory bar3 = (Memory)foo3;
  pList L3 = ((pList)((void*)foo3-sizeof(list)));
  
  void *f = foo3->gc.alloc(bar3, "Ris-Otto maler på.");
  (CU_ASSERT(L3->first->size == sizeof(int) + sizeof(long)));
  
  // Cleaning the memory from allocated objects  
  freeMem(bar3, f);
}


void testBUGG(void)
{
  Manual foo = (Manual)iMalloc(1000 Mb, MANUAL + ASCENDING_SIZE);
  Memory bar = (Memory)foo;
  pList L = ((void*)foo)-sizeof(list);
  
  //Kontrolerar om det allokerats 1000 Mb
  (CU_ASSERT(foo->avail(bar) == 1000 Mb));
  int i = 0;
  //Skapar 1000 chunkes
  while(i!=1000){
    foo->alloc(bar, 1 Mb);
    i++;
  }
  //Anropar collect som bör sätta alla mark-bits 0
  collect(bar);
  i = 0;
  int mark = 0;
  //Adderar mark-biten på alla 1000 chunkes
  while(i!=1000){
    mark = mark + L->current->mark;
    L->current = L->current->next;
    i++;
  }
  //Denna CU_ASSERT borde returnera fel då mark bör vara lika med 0
  (CU_ASSERT(mark != 0));
}

void testIMALLOC_MANUAL (void) {
  
  // TEST OF ASCENDING SIZE SORT
  Manual fooAsc = (Manual)iMalloc(1 Mb, MANUAL + ASCENDING_SIZE);
  Memory barAsc = (Memory)fooAsc;
  pList LAsc = ((void*)fooAsc)-sizeof(list);

  // Storing an integer in memory barAsc
  int *intAsc = fooAsc->alloc(barAsc, 1 Kb);
  *intAsc = 1337;

  // Storing a string in memory barAsc
  char **stringAsc = fooAsc->alloc(barAsc, 6);
  *stringAsc = "Hejsan";

  // Storing a character in memory barAsc
  char *charAsc = fooAsc->alloc(barAsc, 1);
  *charAsc = 'x';

  // Storing a float in memory barAsc
  float *floatAsc = fooAsc->alloc(barAsc, 10);
  *floatAsc = 24;

  (CU_ASSERT(*intAsc == 1337));
  (CU_ASSERT(*stringAsc == "Hejsan"));
  (CU_ASSERT(*charAsc == 'x'));
  (CU_ASSERT(*floatAsc == 24.000000));
  
  
  // TEST OF DESCENDING SIZE SORT
  Manual fooDesc = (Manual)iMalloc(500 Kb, MANUAL + DESCENDING_SIZE);
  Memory barDesc = (Memory)fooDesc;
  pList LDesc = ((void*)fooDesc)-sizeof(list);

  // Storing an integer in memory barDesc
  int *intDesc = fooDesc->alloc(barDesc, 2 Kb);
  *intDesc = 8520;

  // Storing a string in memory barDesc
  char **stringDesc = fooDesc->alloc(barDesc, 10);
  *stringDesc = "Ola";

  // Storing a character in memory barDesc
  char *charDesc = fooDesc->alloc(barDesc, 1);
  *charDesc = 'g';

  // Storing a float in memory barDesc
  float *floatDesc = fooDesc->alloc(barDesc, 9);
  *floatDesc = 18;

  (CU_ASSERT(*intDesc == 8520));
  (CU_ASSERT(*stringDesc == "Ola"));
  (CU_ASSERT(*charDesc == 'g'));
  (CU_ASSERT(*floatDesc == 18.000000));


  // TEST OF ADDRESS SORT
  Manual fooAdd = (Manual)iMalloc(1 Mb, MANUAL + ADDRESS);
  Memory barAdd = (Memory)fooAdd;
  pList LAdd = ((void*)fooAdd)-sizeof(list);

  // Storing an integer in memory barAdd
  int *intAdd = fooAdd->alloc(barAdd, 16);
  *intAdd = 1234568;

  // Storing a string in memory barAdd
  char **stringAdd = fooAdd->alloc(barAdd, 18);
  *stringAdd = "Lars gillar glass";

  // Storing a character in memory barAdd
  char *charAdd = fooAdd->alloc(barAdd, 1);
  *charAdd = 'r';

  // Storing a float in memory barAdd
  float *floatAdd = fooAdd->alloc(barAdd, 20);
  *floatAdd = 18;

  (CU_ASSERT(*intAdd == 1234568));
  (CU_ASSERT(*stringAdd == "Lars gillar glass"));
  (CU_ASSERT(*charAdd == 'r'));
  (CU_ASSERT(*floatAdd == 18.000000));
}

void testIMALLOC_MANAGED (void) {
  
  // TEST OF REFCOUNT + ASCENDING SIZE SORT
  Managed fooRef = (Managed)iMalloc(10 Kb, REFCOUNT + ASCENDING_SIZE);
  Memory barRef = (Memory)fooRef;
  pList LRef = ((void*)fooRef)-sizeof(list);

  // Storing an integer in memory barRef
  int *intRef = fooRef->alloc(barRef, 16);
  *intRef = 1234568;

  // Storing a string in memory barRef
  char **stringRef = fooRef->alloc(barRef, 23);
  *stringRef = "Lars gillar inte glass";

  // Storing a character in memory barRef
  char *charRef = fooRef->alloc(barRef, 1);
  *charRef = 'r';

  // Storing a float in memory barRef
  float *floatRef = fooRef->alloc(barRef, 20);
  *floatRef = 18;

  (CU_ASSERT(*intRef == 1234568));
  (CU_ASSERT(*stringRef == "Lars gillar inte glass"));
  (CU_ASSERT(*charRef == 'r'));
  (CU_ASSERT(*floatRef == 18.000000));


  // TEST OF GARBAGE COLLECT + DESCENDING SIZE
  Managed fooGC = (Managed)iMalloc(10 Kb, GCD + DESCENDING_SIZE);
  Memory barGC = (Memory)fooGC;
  pList LGC = ((void*)fooGC)-sizeof(list);

  // Storing an integer in memory barGC
  int *intGC = fooGC->alloc(barGC, 16);
  *intGC = 4;

  // Storing a string in memory barGC
  char **stringGC = fooGC->alloc(barGC, 39);
  *stringGC = "Verkar som att Lars inte gillar glass";

  // Storing a character in memory barGC
  char *charGC = fooGC->alloc(barGC, 1);
  *charGC = 'r';

  // Storing a float in memory barGC
  float *floatGC = fooGC->alloc(barGC, 20);
  *floatGC = 4;

  (CU_ASSERT(*intGC == 4));
  (CU_ASSERT(*stringGC == "Verkar som att Lars inte gillar glass"));
  (CU_ASSERT(*charGC == 'r'));
  (CU_ASSERT(*floatGC == 4.000000));

  // TEST OF REFCOUNT + GARBAGE COLLECT + ADDRESS
  Managed fooRefGC = (Managed)iMalloc(5 Kb, REFCOUNT + GCD + DESCENDING_SIZE);
  Memory barRefGC = (Memory)fooRefGC;
  pList LRefGC = ((void*)fooRefGC)-sizeof(list);

  // Storing an integer in memory barRefGC
  int *intRefGC = fooRefGC->alloc(barRefGC, 16);
  *intRefGC = 0;

  // Storing a string in memory barRefGC
  char **stringRefGC = fooRefGC->alloc(barRefGC, 37);
  *stringRefGC = "70% of the time, it works every time";

  // Storing a character in memory barRefGC
  char *charRefGC = fooRefGC->alloc(barRefGC, 1);
  *charRefGC = 'z';

  // Storing a float in memory barRefGC
  float *floatRefGC = fooRefGC->alloc(barRefGC, 20);
  *floatRefGC = 4;

  (CU_ASSERT(*intRefGC == 0));
  (CU_ASSERT(*stringRefGC == "70% of the time, it works every time"));
  (CU_ASSERT(*charRefGC == 'z'));
  (CU_ASSERT(*floatRefGC == 4.000000)); 
}

int main()
{
  SET_STACK_BOTTOM;	
  CU_pSuite pSuite = NULL;
  CU_pSuite buggSuite = NULL;
  CU_pSuite piMallocSuite = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  /* add a suite to the registry */
  pSuite = CU_add_suite("iMalloc Suite", init_suite, clean_suite);
  buggSuite = CU_add_suite("iMalloc Suite", init_suite, clean_suite);
  piMallocSuite = CU_add_suite("iMalloc Suite", init_suite, clean_suite);
  
  if (NULL == pSuite || buggSuite == NULL || piMallocSuite == NULL)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }


  /* add the tests to the suite */
  if (
      (NULL == CU_add_test(pSuite, "alloc() (ASCENDING SIZE)", testALLOC_ASCENDING_SIZE)) ||
      (NULL == CU_add_test(pSuite, "alloc() (DESCENDING SIZE)", testALLOC_DESCENDING_SIZE)) ||
      (NULL == CU_add_test(pSuite, "alloc() (ADDRESS)", testALLOC_ADDRESS))||
      (NULL == CU_add_test(pSuite, "Test of REFCOUNT + ASCENDING_SIZE", testRC_ASCENDING))||
      (NULL == CU_add_test(pSuite, "Test of REFCOUNT + DESCENDING_SIZE", testRC_DESCENDING))||
      (NULL == CU_add_test(pSuite, "Test of REFCOUNT + ADDRESS", testRC_ADDRESS))||
      (NULL == CU_add_test(pSuite, "Test of REFCOUNT(+GC) + ASCENDING_SIZE", testRC_GC_ASCENDING))||
      (NULL == CU_add_test(pSuite, "Test of REFCOUNT(+GC) + DESCENDING_SIZE", testRC_GC_DESCENDING))||
      (NULL == CU_add_test(pSuite, "Test of REFCOUNT(+GC) + ADDRESS", testRC_GC_ADDRESS))||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+ ASCENDING) one chunk", testCOLLECT_ASCENDING_GC_ONE)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+ ASCENDING) two chunks", testCOLLECT_ASCENDING_GC_TWO)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+ ASCENDING) several chunks", testCOLLECT_ASCENDING_GC_LOTS)) |
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+ ADDRESS) one chunk", testCOLLECT_ADDRESS_GC_ONE)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+ ADDRESS) two chunks", testCOLLECT_ADDRESS_GC_TWO)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+ ADDRESS) several chunks", testCOLLECT_ADDRESS_GC_LOTS)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+DESCENDING) one chunk", testCOLLECT_DESCENDING_GC_ONE)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+DESCENDING) two chunks", testCOLLECT_DESCENDING_GC_TWO)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+DESCENDING) several chunks", testCOLLECT_DESCENDING_GC_LOTS)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC + ASCENDING) one chunk", testCOLLECT_ASCENDING_GC_RC_ONE)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC + ASCENDING) two chunks", testCOLLECT_ASCENDING_GC_RC_TWO)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC + ASCENDING) several chunks", testCOLLECT_ASCENDING_GC_RC_LOTS)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC + ADDRESS) one chunk", testCOLLECT_ADDRESS_GC_RC_ONE)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC + ADDRESS) two chunks", testCOLLECT_ADDRESS_GC_RC_TWO)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC + ADDRESS) several chunks", testCOLLECT_ADDRESS_GC_RC_LOTS)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC +DESCENDING) one chunk", testCOLLECT_DESCENDING_GC_RC_ONE)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC +DESCENDING) two chunks", testCOLLECT_DESCENDING_GC_RC_TWO)) ||
      (NULL == CU_add_test(pSuite, "Test of COLLECT(+RC +DESCENDING) several chunks", testCOLLECT_DESCENDING_GC_RC_LOTS)) ||
      (NULL == CU_add_test(pSuite, "alloc() (FORMAT STRING)", testALLOCCHAR)) ||
      (NULL == CU_add_test(buggSuite, "Test of bugg, if passed the bugg is present", testBUGG)) ||
      (NULL == CU_add_test(piMallocSuite, "iMalloc (MANUAL)", testIMALLOC_MANUAL)) || 
      (NULL == CU_add_test(piMallocSuite, "iMalloc (MANAGED)", testIMALLOC_MANAGED))
      
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
