CC = gcc

source = ./src/main.c ./src/backend

mainpc: main.o backend.o pc.o
	$(CC) ./build/main.o ./build/backend.o ./build/pc.o -o $@ -lmingw32 -lSDL2 -lSDL2main -ggdb3

maincalc: main.o backend.o calc.o
	$(CC) ./build/main.o ./build/backend.o ./build/calc.o -o $@ -ggdb3

%.o : ./src/%.c
	$(CC) -c $^ -o ./build/$@ -ggdb3

%.o : ./src/backends/%.c
	$(CC) -c $^ -o ./build/$@ -ggdb3
