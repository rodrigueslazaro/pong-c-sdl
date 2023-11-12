build:
	gcc -Wall -std=c99 ./*.c -lSDL2 -o game
run:
	./game
clean:
	rm game