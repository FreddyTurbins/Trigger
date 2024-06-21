CC=gcc
CCWIN32=x86_64-w64-mingw32-gcc
cflags=-Wall -Wextra -pedantic -ggdb -I "include/"
src = src/*.c

linux:
	@$(CC) $(cflags) -fPIC -c $(src) -lm -ldl -lpthread -DPLATFORM_DESKTOP
	@$(CC) *.o -o libtrigger.a -shared
	@rm *.o

win32:
	@$(CCWIN32) $(cflags) -fPIC -c $(src) -lm -DPLATFORM_DESKTOP
	@$(CCWIN32) *.o -o libtrigger.dll -lwinmm -lgdi32 -shared
	@rm *.o
	
