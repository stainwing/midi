#ifndef TRAMO_H
#define TRAMO_H
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

void imprimir_muestras(const float v[], size_t n, double t0, int f_m);

void muestrear_senoidal(float v[], size_t n, double t0, int f_m, float f, float a);

void muestrear_armonico(float v[], size_t n, double t0, int f_m, float f, float a, float fa[][2], size_t n_fa);

//tipo de dato que representa los datos de un tramo de una onda de sonid
typedef struct{
	float *v; //vector dinamico que contiene los valores en flotantes de una onda
	size_t n; //cantidad de muestras de la onda
	double t0; //tiempo de inicio de la onda
	int f_m; //frecuencia de muestreo
}tramo_t;

void tramo_destruir(tramo_t *t);

tramo_t *tramo_crear_muestreo(double t0, double tf, int f_m, float f, float a, float fa[][2], size_t n_fa);

bool tramo_redimensionar(tramo_t *t, double tf);

bool tramo_extender(tramo_t *destino, const tramo_t *extension);


#endif