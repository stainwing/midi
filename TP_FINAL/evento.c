#include "evento.h"

#define MASK_EVENTO 0x70
#define MASK_VALIDAR_EVENTO 0x80
#define MASK_CANAL 0x0f
#define SHIFT_EVENTO 4
#define SHIFT_CANAL 0
#define SHIFT_VALIDAR_EVENTO 7


const struct eventos eventos[]={
	{2, "Nota apagada"},
	{2, "Nota encendida"},
	{2, "Nota durante"},
	{2, "Cambio de control"},
	{1, "Cambio de programa"},
	{1, "Variar canal"},
	{2, "Cambio de pitch"},
	{2, "Metaevento"}
};

bool decodificar_evento(uint8_t valor, evento_t *evento, char *canal, int *longitud){
	if(!(valor & MASK_VALIDAR_EVENTO))
		return false;

	*evento = (valor & MASK_EVENTO)>>SHIFT_EVENTO;
	*canal = (valor & MASK_CANAL)>>SHIFT_CANAL;
	*longitud = eventos[*evento].lon;
	return true;
}