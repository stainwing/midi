#ifndef NOTAS_MIDI_H
#define NOTAS_MIDI_H

#include <stdio.h>
#include <stdint.h>

//estructura que guarda de que nota se trata, de que octava, el tiempo en que se inicia dicha nota y el tiempo en que finaliza la nota y la amplitud de la nota 
typedef struct notas{
    nota_t nota; 
    signed char octava;
    double t0; //tiempo en que se enciende la nota
    double tf; //tiempo en que se apaga la nota
    uint8_t velocidad; //amplitud de la nota
}datos_notas_t;

bool leer_midi(FILE* f, char canal_recibido,int pps, datos_notas_t notas[], size_t n_notas,size_t *notas_leidas);

#endif