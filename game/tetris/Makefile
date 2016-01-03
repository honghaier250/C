CC=gcc -g

tetris: main.o block.o
	gcc -g -o tetris main.o block.o
%.c:%.o
	gcc -g -c $@ $<

.PHONY:clean

clean:
	rm -rf main.o block.o tetris
