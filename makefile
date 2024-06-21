CC=gcc
CCWIN32=x86_64-w64-mingw32-gcc
cflags=-Wall -Wextra -pedantic -ggdb -I "include/"
src = src/tecore.c

linux: build
	@$(CC)	-fPIC -c src/teglfw.c -lm -ldl -lpthread
	@$(CC) $(cflags) -fPIC -c $(src) -lm -ldl -lpthread -DPLATFORM_DESKTOP
	@$(CC) *.o -o build/libtrigger.a -shared
	@rm *.o

win32: build
	@$(CCWIN32)	-fPIC -c src/teglfw.c -lm -lwinmm -lgdi32
	@$(CCWIN32) $(cflags) -fPIC -c $(src) -lm -DPLATFORM_DESKTOP
	@$(CCWIN32) *.o -o build/libtrigger.dll -lwinmm -lgdi32 -shared
	@rm *.o

build:
	ifneq ($(wildcard build),)
		@echo "Found build/"
	else
		@echo "Creating build/" && mkdir build
	endif
