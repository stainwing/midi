CFLAGS=-Wall -std=c99 -pedantic -c -g -O2 -lm

sintetizador: main.o sintetizador.o lectura_midi.o evento.o formato.o wave.o nota.o tramo.o notas_midi.o
	gcc main.o sintetizador.o lectura_midi.o evento.o formato.o wave.o nota.o tramo.o notas_midi.o -o sintetizador -lm -g

main.o: nota.h wave.h notas_midi.h tramo.h sintetizador.h main.c
		gcc $(CFLAGS) main.c

sintetizador.o: sintetizador.h nota.h notas_midi.h tramo.h sintetizador.c
	gcc $(CFLAGS) sintetizador.c

lectura_midi.o: evento.h nota.h formato.h lectura_midi.h lectura_midi.c
	gcc $(CFLAGS) lectura_midi.c	

evento.o: evento.h evento.c
	gcc $(CFLAGS) evento.c

formato.o: formato.h formato.c
	gcc $(CFLAGS) formato.c	

wave.o: wave.h tramo.h wave.c
	gcc $(CFLAGS) wave.c	

nota.o: nota.h nota.c
	gcc $(CFLAGS) nota.c

tramo.o: tramo.h tramo.c
	gcc $(CFLAGS) tramo.c

notas_midi.o: notas_midi.c notas_midi.h formato.h evento.h nota.h lectura_midi.h
	gcc $(CFLAGS) notas_midi.c