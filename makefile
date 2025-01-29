CC=gcc
CCWIN32=x86_64-w64-mingw32-gcc
cflags=-Wall -Wextra -pedantic -ggdb -I "include/"
src = src/tcore.c src/trenderer.c src/utils.c

linux:
	@$(CC)	-fPIC -c src/tglfw.c src/vendor/glad/glad.c -lm -ldl -lpthread -D_GNU_SOURCE
	@$(CC) $(cflags) -fPIC -c $(src) -lm -ldl -lpthread -DPLATFORM_DESKTOP
	@$(CC) *.o -o build/libtrigger.a -shared -lm
	@rm *.o

win32:
	@$(CCWIN32)	-fPIC -c src/tglfw.c src/vendor/glad/glad.c -lm
	@$(CCWIN32) $(cflags) -fPIC -c $(src) -lm -DPLATFORM_DESKTOP
	@$(CCWIN32) *.o -o build/libtrigger.dll -lwinmm -lgdi32 -shared
	@rm *.o

linuxexamples: linux
	@echo COMPILING HELLO TRIANGLE
	@$(CC) $(cflags) -L./build/	-l:libtrigger.a -o ./build/hellotriangle examples/hellotriangle/main.c -Wl,-rpath,./
	@$(CC) $(cflags) -L./build/	-l:libtrigger.a -o ./build/rectangle examples/rectangle/main.c -Wl,-rpath,./
	@$(CC) $(cflags) -L./build/	-l:libtrigger.a -o ./build/circle examples/circle/main.c -Wl,-rpath,./
	@$(CC) $(cflags) -L./build/	-l:libtrigger.a -o ./build/texture examples/texture/main.c -Wl,-rpath,./
	@$(CC) $(cflags) -L./build/	-l:libtrigger.a -o ./build/shader examples/shader/main.c -Wl,-rpath,./

win32ex: win32
	@echo COMPILING HELLO TRIANGLE
	@$(CCWIN32) $(cflags) examples/hellotriangle/main.c -o ./build/hellotriangle.exe -L./build/ -l:libtrigger.dll -static
	@$(CCWIN32) $(cflags) examples/rectangle/main.c -o ./build/rectangle.exe -L./build/ -l:libtrigger.dll -static
	@$(CCWIN32) $(cflags) examples/texture/main.c -o ./build/texture.exe -L./build/ -l:libtrigger.dll -static
