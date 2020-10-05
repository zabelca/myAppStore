CC=gcc
CFLAGS=-I.
DEPS = myAppStore.h
OBJ = myAppStore.o test.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

myAppStore: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

check: test
	make && ./test
