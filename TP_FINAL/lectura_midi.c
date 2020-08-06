#include "formato.h"
#include "nota.h"
#include "evento.h"
#include "lectura_midi.h"
#define MASK_VALIDAR_EVENTO 0x80
#define HEADER_ID 0x4d546864
#define TRACK_ID 0x4d54726b
#define MASK_TIEMPO 0x7f

//funciones para leer una cantidad de bytes indicada en bigendian
uint8_t leer_uint8_t(FILE *f){
	uint8_t dato;
	size_t n = fread(&dato, sizeof(uint8_t), 1, f);
	if(n != 1) return 0;
	return dato;
}

//recibe un archivo previamente abierto y devuelve uint16_t bigendian
uint16_t leer_uint16_bigendian(FILE *f){
	uint8_t datos[2];
	uint16_t resultado=0;
	size_t n = fread(&datos, sizeof(uint8_t), sizeof(uint16_t), f);
	for(size_t i=0; i<n;i++)
		resultado = (resultado <<8) | datos[i];
	return resultado;
}

//recibe un archivo previamente abierto y devuelve un uint32_t bigendian
uint32_t leer_uint32_bigendian(FILE *f){
	uint8_t datos[4];
	uint32_t resultado=0;
	size_t n = fread(&datos, sizeof(uint8_t), sizeof(uint32_t), f);
	for(size_t i=0; i<n; i++)
		resultado = (resultado << 8) | datos[i];
	return resultado;
}

//funcion para leer el encabezado del archivo.midi y validar que se trata del mismo
bool leer_encabezado(FILE *f, formato_t *formato, uint16_t *numero_pistas, uint16_t *pulsos_negra){
	uint32_t validar_header_id=leer_uint32_bigendian(f);
	if(validar_header_id != HEADER_ID)
		return false;

	uint32_t size=leer_uint32_bigendian(f);
	if(size!=6)
		return false;

	uint16_t valor=leer_uint16_bigendian(f);
	if(!(decodificar_formato(valor, formato)))
		return false;

	*numero_pistas=leer_uint16_bigendian(f);
	*pulsos_negra=leer_uint16_bigendian(f);

	return true;
}

//funcion para leer el tamaño de una pista
bool leer_pista(FILE *f, uint32_t *tamagno){
	uint32_t validar_track_id = leer_uint32_bigendian(f);
	if(validar_track_id != TRACK_ID)
		return false;

	*tamagno= leer_uint32_bigendian(f);
	return true;
}

//lee del archivo.mid el tiempo en que se desarrolla un evento
bool leer_tiempo(FILE *f, uint32_t *tiempo){
	uint32_t r=0;
	uint8_t aux=0;
	int i=0;
	
	while((aux=leer_uint8_t(f)) & MASK_VALIDAR_EVENTO){
		if(i > 3)
			return false;

		aux &= MASK_TIEMPO;
		
		r= (r << 7) | aux;
		i++;
	}

	*tiempo = (r << 7) |aux;
	return true;

}

//lee del archivo.mid el evento que se esta produciendo, en que canal y su mensaje
bool leer_evento(FILE *f, evento_t *evento, char *canal, int *longitud, uint8_t mensaje[]){
	uint8_t dato=leer_uint8_t(f);
	
	if(!(decodificar_evento(dato, evento, canal, longitud))){
		mensaje[0]=dato;
		for(size_t i=0; i < (*longitud - 1); i++)
			mensaje[i+1] = leer_uint8_t(f);
		return true;
	}

	for(size_t i=0; i < (*longitud); i++)
		mensaje[i]=leer_uint8_t(f);
	return true;
}

void descartar_metaevento(FILE *f, uint8_t tamagno){
	for(size_t i=0; i < tamagno; i++)
		leer_uint8_t(f);
}
