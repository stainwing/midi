#include "formato.h"

bool decodificar_formato(uint16_t valor, formato_t *formato){
	if(valor > MULTIPISTAS_ASINCRONICAS)
		return false;
	
	*formato=valor;
	return true;
}