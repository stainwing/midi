#include "nota.h"

#define MASK_VALIDAR_EVENTO 0x80
#define F_REF 440
#define OCTAVA_REF 4

bool decodificar_nota(uint8_t valor, nota_t *nota, signed char *octava){
	if(valor & MASK_VALIDAR_EVENTO)
		return false;
	
	*nota = valor % 12;
	*octava = -1 + valor / 12;
	return true;
}

//calcula la frecuencia de la nota dada su nota y su octava
float frecuencia_nota(nota_t nota, signed char octava){
	signed char o = octava - OCTAVA_REF;
	signed char s = nota - LA; 
	return F_REF * pow(2, o + s / (float)12);
}
