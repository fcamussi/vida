
CC = gcc -Wall
CFLAGS = `pkg-config --cflags gtk+-2.0`
LIBS = `pkg-config --libs gtk+-2.0`


vida: main.o window.o bars.o functions.o grid.o vida.o
	$(CC) -o vida main.o window.o bars.o functions.o grid.o vida.o $(LIBS)

main.o: main.c window.h
	$(CC) $(CFLAGS) -c main.c

window.o: window.c bars.h grid.h
	$(CC) $(CFLAGS) -c window.c

bars.o: bars.c functions.h grid.h
	$(CC) $(CFLAGS) -c bars.c

functions.o: functions.c grid.h misc.h
	$(CC) $(CFLAGS) -c functions.c

grid.o: grid.c vida.h misc.h
	$(CC) $(CFLAGS) -c grid.c

vida.o: vida.c misc.h
	$(CC) $(CFLAGS) -c vida.c


clean:
	rm -f *.o vida
