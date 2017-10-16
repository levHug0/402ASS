cc=gcc
MAKE=MAKE
RM =rm

all: c.o s.o myheader.o
	$(cc) s.o myheader.o -std=gnu99 -o server
	$(cc) myheader.o c.o -std=gnu99 -o client

c.o: c.c
	$(cc) -c -std=gnu99 c.c

s.o: s.c
	$(cc) -c -std=gnu99 s.c

myheader.o: myheader.c myheader.h
	$(cc) -c -std=gnu99 myheader.c

clean:
	$(RM) *.o server client
