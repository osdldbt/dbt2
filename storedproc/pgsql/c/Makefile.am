AUTOMAKE_OPTIONS = no-dependencies

noinst_PROGRAMS = funcs.so

%.o:%.c
	$(CC) -c -fpic $(CFLAGS) -o $@ $<

funcs.so: funcs.o
	$(CC) -shared -o $@ $<

clean:
	rm -f funcs.so funcs.o

