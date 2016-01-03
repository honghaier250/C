
CC = gcc

all: align button buttonlabel checkbox listbox radio toggle

align: align.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` align.c -o align

button: button.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` button.c -o button

buttonlabel: buttonlabel.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` buttonlabel.c -o buttonlabel

checkbox: checkbox.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` checkbox.c -o checkbox

listbox: listbox.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` listbox.c -o listbox

radio: radio.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` radio.c -o radio

toggle: toggle.c 
	$(CC) `pkg-config --libs --cflags gtk+-2.0` toggle.c -o toggle

clean: 
	rm -f *.o align button buttonlabel checkbox listbox radio toggle
