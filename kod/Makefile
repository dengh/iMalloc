all: make

# Tex settings
C_COMPILER   = gcc
C_OPTIONS    = -ggdb -std=c99

# Clean settings
GEN_EXTENSIONS = *.o *.out

# SCM
VC_PROGRAM = hg

clean:
	rm -f $(GEN_EXTENSIONS) unittests *.orig

beautify:
	astyle -A7 *.c *.h
	
make:	
	$(C_COMPILER) $(C_OPTIONS) -c refcount.c imalloc.c memoryAllocator.c collect.c rootset.c

test:
	$(C_COMPILER) $(C_OPTIONS) unittests.c refcount.c imalloc.c memoryAllocator.c collect.c rootset.c -lcunit -o imalloc
	./imalloc