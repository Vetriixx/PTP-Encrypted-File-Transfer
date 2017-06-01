CC=gcc
CFLAGS=-Wall

make: files.o io.o logfile.o
	$(CC) $(CFLAGS) -o all files.o io.o logfile.o

files.o: files.c files.h
	$(CC) $(CFLAGS) -c files.c

io.o: io.c io.h
	$(CC) $(CFLAGS) -c io.c

logfile.o:  logfile.c logfile.h
	$(CC) $(CFLAGS) -c logfile.c

networking.o:  networking.c networking.h
	$(CC) $(CFLAGS) -c networking.c -lpthread

all: files.c io.c logfile.c networking.c files.h io.h logfile.h networking.h
	$(CC) $(CFLAGS) -o files files.c io.c logfile.c networking.c -lpthread

clean:
	rm -vf *.o io files all *.conf *.txt |mv ./bin* ../ | rmdir ./bin ./Downloads




