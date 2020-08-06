#ifndef FORMATO_H
#define FORMATO_H

#include <stdint.h>
#include <stdbool.h>


typedef enum formato{
	PISTA_UNICA, MULTIPISTAS_SINCRONICAS, MULTIPISTAS_ASINCRONICAS
}formato_t;


bool decodificar_formato(uint16_t valor, formato_t *formato);

const char *codificar_formato(formato_t formato);

#endif