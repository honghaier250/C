
CC = gcc

form: combos.c \
      edit.c \
      combobox.c \
      menucombo.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` \
        combos.c \
        edit.c \
        combobox.c \
        menucombo.c \
        -o form

clean: 
	rm -f *.o form
