EXEC=hello

all: kapture

kapture:
	gcc -o build/kapture src/*.c -lSDL2

run: all
	build/kapture

