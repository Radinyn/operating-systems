CC=gcc
CFLAGS=-Wall
LIB=libwc

all:
	make lib_static
	make lib_shared

lib_static: $(LIB).c
	$(CC) $(CFLAGS) -c $(LIB).c
	ar cr $(LIB).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) -c -fPIC $(LIB).c
	$(CC) $(CFLAGS) -shared -o $(LIB).so $(LIB).o

clean:
	rm -f *.o *.gch *.a *.so