cc=gcc
MAKE=MAKE
RM =rm

all: c.c s.c
	$(cc) -o c c.c -std=gnu99
	$(cc) -o s s.c -std=gnu99

clean: server client
	$(RM) s
	$(RM) c

