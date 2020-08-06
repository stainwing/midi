#include "tramo.h"
#define PI 3.14159265358979

//funcion de onda
double onda(double t, float a, float f, float phi){
	return a * sin(2 * PI * f * t + phi);
}

//inicializa todas las muestras de un vector de flotante en 0
void inicializar_muestras(float v[], size_t n){
	for(size_t i=0; i<n; i++){
		v[i]=0;
	}
}

//devuelve en el vector una cantidad de n muestras de una onda senoidal dado los valores del tiempo inicial, frecuencia de muestreo, frecuencia, y amplitud
void muestrear_senoidal(float v[], size_t n, double t0, int f_m, float f, float a){
	for(size_t i=0; i<n; i++){
		double ti = t0 + (double)i/f_m;
		v[i]+= onda(ti, a, f, 0);
	}
}

//devuelve en el vector una cantidad de n muestras de una onda senoidal dado los valores del tiempo inicial, frecuencia de muestreo, frecuencia, y amplitud
//y una cantidad de armonicos
void muestrear_armonico(float v[], size_t n, double t0, int f_m, float f, float a, float fa[][2], size_t n_fa){
	inicializar_muestras(v, n);
	for(size_t i=0; i<n_fa; i++){
		muestrear_senoidal(v, n, t0, f_m, f*fa[i][0], a*fa[i][1]);
	}
}

//crea un tramo de una onda
static tramo_t *_tramo_crear(double t0, double tf, int f_m){
	tramo_t *tramo=malloc(sizeof(tramo_t));
	if(tramo==NULL)
		return NULL;
	size_t n=(tf-t0)*f_m+1;
	float *v=malloc(n*sizeof(float));
	if(v==NULL){
		free(tramo);
		return NULL;
	}
	tramo->v=v, tramo->n=n, tramo->t0=t0, tramo->f_m=f_m;
	return tramo;
}

//libera la memoria asociada al arreglo de floats que contiene y la memoria del tramo
void tramo_destruir(tramo_t *t){
	free(t->v);
	free(t);
}

tramo_t *tramo_clonar(const tramo_t *t){
	double tf = t->t0 + (double)(t->n - 1)/t->f_m;
	tramo_t *t_clonado=_tramo_crear(t->t0, tf, t->f_m);
	if(t_clonado == NULL) return NULL;
	for(size_t i=0; i < t->n; i++){
		t_clonado->v[i] = t->v[i];
	}
	return t_clonado;
}

tramo_t *tramo_crear_muestreo(double t0, double tf, int f_m, float f, float a, float fa[][2], size_t n_fa){
	tramo_t *t=_tramo_crear(t0, tf, f_m);
	if(t==NULL){
		return NULL;
	}
	muestrear_armonico(t->v, t->n, t0, f_m, f, a, fa, n_fa);
	return t;
}

bool tramo_redimensionar(tramo_t *t, double tf){
	size_t nuevo=(tf - t->t0)*(t->f_m)+1;
	float *vaux = realloc(t->v, (nuevo+1)*sizeof(float));
	if(vaux==NULL)
		return false;
	if(t->n < nuevo){
		for(size_t i=t->n; i < nuevo; i++){
			vaux[i]=0;
		}
	}
	t->v=vaux, t->n=nuevo;
	return true;
}

bool tramo_extender(tramo_t *destino, const tramo_t *extension){
	if(destino->t0 > extension->t0)
		return false;
	double tf_dest= destino->t0 + (double)(destino->n - 1)/destino->f_m;
	double tf_ext= extension->t0 + (double)(extension->n - 1)/destino->f_m;
	if(tf_dest < tf_ext){
		if(tramo_redimensionar(destino, tf_ext) == 0){
			return false;
		}
	}
	double ti=0;
	for(size_t i=0; i < destino->n; i++){
		ti = destino->t0 + (double)(i)/destino->f_m;
		if(ti == extension->t0){
			for(size_t j=0; j < extension->n; j++, i++){
				destino->v[i] += extension->v[j];
			}
		}
	}
	return true;
}