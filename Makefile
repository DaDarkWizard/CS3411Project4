CC = gcc
CFLAGS = -g -Wall -std=c99 -D_POSIX_C_SOURCE
DEPS = 
OBJ = driver.o scanner.o evenodd.o
MAIN = driver scanner evenodd even odd
BUILD =  #-lcrypto -lssl

driver: driver.o scanner evenodd
	$(CC) $(CFLAGS) $(BUILD) -o driver driver.o

scanner: scanner.o evenodd
	$(CC) $(CFLAGS) $(BUILD) -o scanner scanner.o

evenodd: evenodd.o
	$(CC) $(CFLAGS) $(BUILD) -o evenodd evenodd.o
	ln -s evenodd even
	ln -s evenodd odd

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJ) $(MAIN)


