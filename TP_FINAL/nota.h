#ifndef NOTA_H
#define NOTA_H

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

//tipo de dato para poder identificar las notas
typedef enum nota{
	DO, DO_SHARP, RE, RE_SHARP, MI, FA, FA_SHARP, SOL, SOL_SHARP, LA, LA_SHARP, SI
}nota_t;

bool decodificar_nota(uint8_t valor, nota_t *nota, signed char *octava);

float frecuencia_nota(nota_t nota, signed char octava);

#endif
