CC = gcc  
MAINC = gtk_background.c
EXEC = gtk_background 

CFLAGS = `pkg-config --cflags --libs gtk+-2.0 gmodule-export-2.0`
main:  
	$(CC)  $(MAINC)  -o $(EXEC) $(CFLAGS)
clean:
	rm $(EXEC) -rf