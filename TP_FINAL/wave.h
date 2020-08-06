#ifndef WAVE_H
#define WAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int16_t *escalar_tramo(tramo_t *t, size_t *n);

bool escribir_wave(FILE *f, int16_t *v,size_t n, int f_m);

#endif