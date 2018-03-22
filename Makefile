EXEC=hello

all: kapture

kapture: directories
	gcc -o bin/kapture src/*.c -lSDL2 -lSDL2_ttf -ggdb

run: all
	bin/kapture

directories:
	mkdir -p bin
