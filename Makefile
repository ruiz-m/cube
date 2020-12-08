app: display.o main.o
	gcc -o ./app display.o main.o -lSDL2main -lSDL2 -lm

display.o: display.c
	gcc -c display.c

main.o: main.c
	gcc -c main.c

clean:
	rm *.o app
