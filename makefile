CC=gcc
CFLAGS=-I.
DEPS = myAppStore.h
OBJ = myAppStore.o
TESTOBJ = test.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

myAppStore: $(OBJ) main.o
	$(CC) -o $@ $^ $(CFLAGS)

test: $(TESTOBJ) $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

check: test
	@date
	@./test

.PHONY: clean
clean:
	rm -f $(TESTOBJ) $(OBJ) test myAppStore
