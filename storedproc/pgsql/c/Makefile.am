AUTOMAKE_OPTIONS = no-dependencies

noinst_DATA = funcs.so

%.o:%.c
	$(CC) -c -fpic $(CFLAGS) -o $@ $<

funcs.so: funcs.o
	$(CC) $(LDFLAGS) -shared -static-libgcc -o $@ $<

clean:
	rm -f funcs.so funcs.o

