
all:
	@for f in *.c; \
		do \
			make -f Makefile.simple `expr $$f : '\([^.]*\)\.c'`; \
		done

clean:
	@for f in *.c; \
		do \
			rm -rf `expr $$f : '\([^.]*\)\.c' `; \
		done
