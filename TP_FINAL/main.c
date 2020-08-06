#include "nota.c"
#include "notas_midi.c"
#include "tramo.c"
#include "wave.c"
#include "sintetizador.c"

#include "lectura_midi.c"
#include "formato.c"
#include "evento.c"
#define F_M_POR_OMISION 44100
#define CANAL_POR_OMISION 0
#define MAX_ARMONICOS 20
#define MAX_NOTAS 5000
#define PPS 300

static void cerrar_archivos(FILE *s, FILE *m, FILE *w){
	fclose(s);
	fclose(m);
	fclose(w);
}

int main(int argc, char *argv[]){
	//validamos que la cantidad minima de argumentos sea correcta
	if(argc < 7){
		fprintf(stderr, "Uso: %s -s <sintetizador.txt> -i <entrada.mid> -o <salida.wav>\n", argv[0]);
		return 1;
	}

	FILE *sintetizador = NULL;
	FILE *midi = NULL;
	FILE *wave = NULL;
	char canal = CANAL_POR_OMISION;
	int f_m = F_M_POR_OMISION;
	int pps = PPS;

	//abrimos los archivos y leemos los parametros opcionales de pulsos por segundo, canal y frecuencia de muestreo
	for(size_t i=0; i<argc; i++){
		if(strcmp(argv[i], "-s") == 0){
			sintetizador = fopen(argv[i+1], "rt");
			if(sintetizador == NULL){
				fprintf(stderr, "No se pudo abrir el sintetizador\n");
				if(midi != NULL)
					fclose(midi);
				if(wave != NULL)
					fclose(wave);
				return 1;
			}
		}	
		if(strcmp(argv[i], "-i") == 0){
			midi = fopen(argv[i+1], "rb");
			if(midi == NULL){
				fprintf(stderr, "No se pudo abrir el midi\n");
				if(sintetizador != NULL)
					fclose(sintetizador);
				if(wave != NULL)
					fclose(wave);
				return 1;
			}		
		}
		if(strcmp(argv[i], "-o") == 0){
			wave = fopen(argv[i+1], "wb");
			if(wave == NULL){
				fprintf(stderr, "No se pudo abrir el wave\n");
				if(sintetizador != NULL)
					fclose(sintetizador);
				if(midi != NULL)
					fclose(midi);
				return 1;
			}	
		}
		if(strcmp(argv[i], "-c") == 0){
			canal = atoi(argv[i+1]);
		}
		if(strcmp(argv[i], "-f") == 0){
			f_m = atoi(argv[i+1]); 
		}
		if(strcmp(argv[i], "-r") == 0){
			pps = atoi(argv[i+1]);
		}
	}

	//leer armonicos del sintetizador
	float fa[MAX_ARMONICOS][2];
	size_t n_fa;

	if(!leer_num_armonicos(sintetizador, &n_fa)){
		printf("Fallo al leer numero de armonicos\n");
		return 1;
	}
		
	if(!leer_armonicos(sintetizador, fa, n_fa)){
		fprintf(stderr, "Fallo al leer armonicos\n");
		cerrar_archivos(sintetizador, midi, wave);
		return 1;
	}

	//leer ataque
	float ataque[3];
	modulacion_t a;
	if(!leer_funcion_modulacion(sintetizador, &a, ataque)){
		fprintf(stderr, "Fallo al leer funcion de ataque\n");
		cerrar_archivos(sintetizador, midi, wave);
		return 1;
	}

	//leer sostenido
	float sostenido[3];
	modulacion_t s;
	if(!leer_funcion_modulacion(sintetizador, &s, sostenido)){
		fprintf(stderr, "Fallo al leer funcion de sostenido\n");
		cerrar_archivos(sintetizador, midi, wave);
		return 1;
	}

	//leer decaimiento
	float decaimiento[3];
	modulacion_t d;
	if(!leer_funcion_modulacion(sintetizador, &d, decaimiento)){
		fprintf(stderr, "Fallo al leer funcion de decaimiento\n");
		cerrar_archivos(sintetizador, midi, wave);
		return 1;
	}

	//leer notas del midi
	datos_notas_t notas[MAX_NOTAS];
	size_t notas_leidas;
	if(!leer_midi(midi, canal, pps, notas, MAX_NOTAS,&notas_leidas)){
		fprintf(stderr, "Fallo al leer notas del midi\n");
		cerrar_archivos(sintetizador, midi, wave);
		return 1;
	}
	
	//sintetizar notas
	tramo_t *t = tramo_crear_muestreo(0, 1, f_m, 0, 0, fa, n_fa);
	if(t == NULL){
		fprintf(stderr, "Fallo al sintetizar notas\n");
		cerrar_archivos(sintetizador, midi, wave);
		return 1;
	}
	for(size_t i=0; i < notas_leidas; i++){
		tramo_t *tramo_nota = crear_onda_nota(notas[i], f_m, fa, n_fa);
		if(tramo_nota == NULL){
			tramo_destruir(t);
			fprintf(stderr, "Fallo al sintetizar notas\n");
			tramo_destruir(t);
			cerrar_archivos(sintetizador, midi, wave);
			return 1;
		}
		if(!sintetizar_nota(tramo_nota, a, ataque, s, sostenido, d, decaimiento)){
			tramo_destruir(t);
			tramo_destruir(tramo_nota);
			fprintf(stderr, "Fallo al sintetizar notas\n");
			cerrar_archivos(sintetizador, midi, wave);
			return 1;
		}
		if(!tramo_extender(t, tramo_nota)){
			tramo_destruir(t);
			tramo_destruir(tramo_nota);
			fprintf(stderr, "Fallo al sintetizar notas\n");
			cerrar_archivos(sintetizador, midi, wave);
			return 1;
		}

		tramo_destruir(tramo_nota);
	}
	printf("Se sintetizaron las notas\n");
	
	size_t n_v;
	int16_t *v = escalar_tramo(t, &n_v);
	if(v == NULL){
		tramo_destruir(t);
		cerrar_archivos(sintetizador, midi, wave);
		tramo_destruir(t);
		return 1;
	}

	//escribir wave
	if(!escribir_wave(wave, v, n_v,f_m)){
		fprintf(stderr, "Fallo al generar el wave\n");
		tramo_destruir(t);
		cerrar_archivos(sintetizador, midi, wave);
		return 1;
	}

	printf("Se genero el wave\n");
	//cerrar archivos
	cerrar_archivos(sintetizador, midi, wave);
	free(v);
	tramo_destruir(t);
	return 0;
}