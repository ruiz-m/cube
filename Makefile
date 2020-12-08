app: display.o main.o
	gcc -o bin/app bin/display.o bin/main.o -lSDL2main -lSDL2 -lm

display.o: src/display.c
	gcc -o bin/display.o -c src/display.c

main.o: src/main.c
	gcc -o bin/main.o -c src/main.c

clean:
	rm bin/*
