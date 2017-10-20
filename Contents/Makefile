cc=gcc
MAKE=MAKE
RM =rm

all: c.o s.o myheader.o
	$(cc) s.o myheader.o -std=gnu99 -o server -pthread
	$(cc) myheader.o c.o -std=gnu99 -o client -pthread

c.o: c.c
	$(cc) -c -std=gnu99 c.c -pthread

s.o: s.c
	$(cc) -c -std=gnu99 s.c -pthread

myheader.o: myheader.c myheader.h
	$(cc) -c -std=gnu99 myheader.c -pthread

clean:
	$(RM) *.o server client
