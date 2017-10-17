cc=gcc
MAKE=MAKE
RM =rm

all: c.c s.c myheader.c
	$(cc) -o s s.c myheader.c -std=gnu99 
	$(cc) -o c c.c myheader.c -std=gnu99

clean:
	$(RM) *.o server client
