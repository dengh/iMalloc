Project made at Uppsala University by:
Engh, Daniel
Karlbom, Hannes
Kjellgren, Fredrik
Lindberg, Erik
Samuelsson, Jonas
Sundberg, Martin

iMalloc is a library much like regular C malloc but with added features.

	iMalloc supports:
		Manual memory management.
		Reference count.
		Automatic garbage collection.
		
		The library is developed for UU Solaris servers and may not work on other computers without adjustments to the source code.

	You can find the source code, the test file and the makefile in the folder named "kod".
	
	Documentation can be found in the folder "Dokumentation". All documentation is in Swedish.
	

Makefile commands:

make - Compiles the project
make test - Compiles and runs unit test (requires CUnit)
make clean - removes output files