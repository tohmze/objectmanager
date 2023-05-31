all: main objectManager.o

main: main.c ObjectManager.h objectManager.o
	clang++ -Wall -g main2.c objectManager.o -o main

objectManager.o: objectManager.c ObjectManager.h
	clang++ -Wall objectManager.c -o objectManager.o -c

clean:
	rm main objectManager.o