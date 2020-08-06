#include "tramo.h"
#include "wave.h"
#define CHUNK_ID "RIFF"
#define FORMAT "WAVE"
#define SUBCHUNK1_ID "fmt "
#define SUBCHUNK2_ID "data"
#define MAX_INT16_T 32767

int16_t *escalar_tramo(tramo_t *t, size_t *n){
	float max = t->v[0];
	for(size_t i=1; i < t->n; i++){
		if(t->v[i] > max)
			max = t->v[i];
	}
	float factor_conversion = MAX_INT16_T / max;
	int16_t *v = malloc(t->n * sizeof(uint16_t));
	if(v == NULL)
		return NULL;
	for(size_t i=0; i < t->n; i++)
		v[i] = t->v[i] * factor_conversion;
	*n = t->n;
	return v;
}

bool escribir_wave(FILE *f, int16_t *v,size_t n, int f_m){
	if(fwrite(CHUNK_ID, sizeof(char), 4, f) !=4 ){
		return false;
	}

	uint32_t chunksize = 36+2*n;
	if(!fwrite(&chunksize,sizeof(uint32_t), 1, f)){
		return false;
	}

	if(fwrite(FORMAT, sizeof(char), 4, f) != 4){
		return false;
	}

	if(fwrite(&SUBCHUNK1_ID, sizeof(char), 4, f) != 4){
		return false;
	}

	uint32_t subchunk1_size = 16;
	if(!fwrite(&subchunk1_size, sizeof(uint32_t), 1, f)){
		return false;
	}

	uint16_t audio_format =  1;
	if(!fwrite(&audio_format, sizeof(uint16_t), 1, f)){
		return false;
	}

	uint16_t numchannels = 1;
	if(!fwrite(&numchannels, sizeof(uint16_t), 1, f)){
		return false;
	}

	uint32_t sample_rate = f_m;
	if(!fwrite(&sample_rate, sizeof(uint32_t), 1, f)){
		return false;
	}

	uint32_t byte_rate = 2*sample_rate;
	if(!fwrite(&byte_rate, sizeof(uint32_t), 1, f)){
		return false;
	}

	uint16_t block_align = 2;
	if(!fwrite(&block_align, sizeof(uint16_t), 1, f)){
		return false;
	}

	uint16_t bits_per_sample = 16;
	if(!fwrite(&bits_per_sample, sizeof(uint16_t), 1, f)){
		return false;
	}

	if(fwrite(SUBCHUNK2_ID, sizeof(char), 4, f) != 4){
		return false;
	}

	uint32_t subchunk2_size = 2*n;
	if(!fwrite(&subchunk2_size, sizeof(uint32_t), 1, f)){
		return false;
	}

	if(fwrite(v, sizeof(int16_t), n, f) != n){
		return false;
	}

	return true;
}