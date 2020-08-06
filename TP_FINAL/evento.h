#ifndef EVENTO_H
#define EVENTO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum evento{
	EVENTO_NOTA_APAGADA, EVENTO_NOTA_ENCENDIDA, EVENTO_NOTA_DURANTE, EVENTO_CAMBIO_DE_CONTROL, EVENTO_CAMBIO_DE_PROGRAMA, EVENTO_VARIAR_CANAL,
	EVENTO_CAMBIO_DE_PITCH, EVENTO_METAEVENTO
}evento_t;

//almacena la longitud del mensaje de un evento y una cadena de caracteres que representa el evento del cual se trata
struct eventos{
	int lon;
	char *significado;
};

bool decodificar_evento(uint8_t valor, evento_t *evento, char *canal, int *longitud);

#endif