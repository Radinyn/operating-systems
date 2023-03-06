SHELL=/bin/bash
CC=gcc
CFLAGS=-Wall
LIB=libwc
REPL=wc_repl.c
BINS=wc_repl.c dllmanager.h

all:
	make lib_static
	make lib_shared
	make exe_static
	make exe_shared
	make exe_dlopen

o:
	make all OPT_FLAGS="-O"

o0:
	make all OPT_FLAGS="-O0"

o1:
	make all OPT_FLAGS="-O1"

o2:
	make all OPT_FLAGS="-O2"

o3:
	make all OPT_FLAGS="-O3"

os:
	make all OPT_FLAGS="-Os"

test:
	sudo mv libwc.so /usr/lib
	make test_create
	./wc_repl_static.exe < test.in > results_static_$(O_SUFFIX).txt 2>/dev/null
	./wc_repl_shared.exe < test.in > results_shared_$(O_SUFFIX).txt 2>/dev/null
	./wc_repl_dlopen.exe < test.in > results_dynamic_$(O_SUFFIX).txt 2>/dev/null

test_create:
	echo "init 1000" > test.in

	(ls -p | grep -v / | sed 's/^/count /') >> test.in

	echo "show 0" >> test.in
	echo "show 1" >> test.in
	echo "show 2" >> test.in

	echo "delete 1" >> test.in
	echo "destroy" >> test.in 
	echo "exit" >> test.in

report:
	sudo ls > /dev/null
	make o
	make test O_SUFFIX="O"
	make o0
	make test O_SUFFIX="O0"
	make o1
	make test O_SUFFIX="O1"
	make o2
	make test O_SUFFIX="O2"
	make o3
	make test O_SUFFIX="O3"
	make os
	make test O_SUFFIX="Os"
	make o3
	rm -f test.in

lib_static: $(LIB).c
	$(CC) $(CFLAGS) $(OPT_FLAGS) -c $(LIB).c
	ar cr $(LIB)$(SUFFIX).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) $(OPT_FLAGS) -c -fPIC $(LIB).c
	$(CC) $(CFLAGS) $(OPT_FLAGS) -shared -o $(LIB).so $(LIB).o

exe_static: $(LIB).c $(BINS)
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(REPL) -o wc_repl_static.exe -L . $(LIB).a

exe_shared: $(LIB).c $(BINS)
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(REPL) -o wc_repl_shared.exe -L . $(LIB).so

exe_dlopen: $(LIB).c $(BINS)
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(REPL) -o wc_repl_dlopen.exe -ldl -D USE_DLL

clean:
	rm -f *.o *.gch *.a *.so *.exe test.in *.txt