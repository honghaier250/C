CC = gcc  
MAINC = glade_test.c
EXEC = glade_test 

CFLAGS = `pkg-config --cflags --libs gtk+-2.0 gmodule-export-2.0`
main:  
	$(CC)  $(MAINC)  -o $(EXEC) $(CFLAGS)
clean:
	rm $(EXEC) -rf