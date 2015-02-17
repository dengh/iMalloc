#include "imalloc.h"
#include <stdio.h>

int main () {
  Manual fooTest = (Manual)iMalloc(1 Kb, MANUAL + ASCENDING_SIZE);
  Memory barTest = (Memory)fooTest;

  // Storing an integer in memory barAsc
  int *intTest = fooTest->alloc(barTest, 10);
  *intTest = 1337;
  printf("intTest: %d\n", *intTest);

  char *charTest = fooTest->alloc(barTest, 10);
  *charTest = 'c';
  printf("charTest: %c\n", *charTest);

  float *floatTest = fooTest->alloc(barTest, 10);
  *floatTest = 0.111;
  printf("floatTest: %f\n",  *floatTest);
  
  char **stringTest = fooTest->alloc(barTest, 10);
  *stringTest = "hejsan";
  printf("stringTest: %s\n\n\n",  *stringTest);






  
  Managed fooTestGC = (Managed)iMalloc(1 Kb, GCD + ASCENDING_SIZE);
  Memory barTestGC = (Memory)fooTestGC;

  int *intTestGC = fooTestGC->alloc(barTestGC, 10);
  *intTestGC = 1337;
  printf("intTestGC: %d\n", *intTestGC);

  char *charTestGC = fooTestGC->alloc(barTestGC, 10);
  *charTestGC = 'c';
  printf("charTestGC: %c\n", *charTestGC);

  float *floatTestGC = fooTestGC->alloc(barTestGC, 10);
  *floatTestGC = 0.111;
  printf("floatTestGC: %f\n",  *floatTestGC);
  
  char **stringTestGC = fooTestGC->alloc(barTestGC, 10);
  *stringTestGC = "hejsan";
  printf("stringTestGC: %s\n\n\n",  *stringTestGC);







  Managed fooTestRefcount = (Managed)iMalloc(1 Kb, REFCOUNT + DESCENDING_SIZE);
  Memory barTestRefcount = (Memory)fooTestRefcount;

  int *intTestRefcount = fooTestRefcount->alloc(barTestRefcount, 10);
  *intTestRefcount = 1337;
  printf("intTestRefcount: %d\n", *intTestRefcount);

  char *charTestRefcount = fooTestRefcount->alloc(barTestRefcount, 10);
  *charTestRefcount = 'c';
  printf("charTestRefcount: %c\n", *charTestRefcount);

  float *floatTestRefcount = fooTestRefcount->alloc(barTestRefcount, 10);
  *floatTestRefcount = 0.111;
  printf("floatTestRefcount: %f\n",  *floatTestRefcount);
  
  char **stringTestRefcount = fooTestRefcount->alloc(barTestRefcount, 10);
  *stringTestRefcount = "hejsan";
  printf("stringTestRefcount: %s\n",  *stringTestRefcount);

  return 0;

}
