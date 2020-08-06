#ifndef SINTETIZADOR_H
#define SINTETIZADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

//tipo de dato que representa la funcion de modulacion leida del archivo.txt
typedef enum{CONSTANT, LINEAR, INVLINEAR, EXP, INVEXP, QUARTCOS, QUARTSIN, HALFCOS,
								HALFSIN, LOG, INVLOG, SIN, TRI, PULSES}modulacion_t;

bool leer_num_armonicos(FILE *f, size_t *n_fa);

bool leer_armonicos(FILE *f, float fa[][2], size_t n_fa);

bool leer_funcion_modulacion(FILE *f, modulacion_t *n, float params[3]);

tramo_t *crear_onda_nota(datos_notas_t nota, int f_m, float fa[][2], size_t n_fa);

bool sintetizar_nota(tramo_t *t, modulacion_t a, float ataque[3], modulacion_t s, float sostenido[3], modulacion_t decay, float decaimiento[3]);

#endif