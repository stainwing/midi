#ifndef EJ5_H
#define EJ5_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

//leer bytes
uint8_t leer_uint8_t(FILE *f);
uint16_t leer_uint16_bigendian(FILE *f);
uint32_t leer_uint32_bigendian(FILE *f);

//encabezado
bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulsos_negra);

//pistas
bool leer_pista(FILE *f, uint32_t *tamagno);

//evento
bool leer_tiempo(FILE *f, uint32_t *tiempo);
bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]);
void descartar_metaevento(FILE *f, uint8_t tamagno);

#endif