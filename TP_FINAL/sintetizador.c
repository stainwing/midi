#include "nota.h"
#include "notas_midi.h"
#include "tramo.h"
#include "sintetizador.h"
#define MAX_CADENA 250
#define MAX 100
#define PI 3.14159265358979


//funciones de modulacion
static float modulacion_constante(double t, float params[3]){
	return 1;
}

static float modulacion_linear(double t, float params[3]){
	float t0 = params[0];
	return t/t0;
}

static float modulacion_invlinear(double t, float params[3]){
	float t0 = params[0];
	float valor = 1 - t/t0;
	if(valor > 0)
		return valor;
	else return 0;
}

static float modulacion_sin(double t, float params[3]){
	float a=params[0], f=params[1];
	return 1+a * sin(f*t);
}

static float modulacion_exp(double t, float params[3]){
	float t0 = params[0];
	return exp(5*(t-t0)/t0);
}

static float modulacion_invexp(double t, float params[3]){
	float t0 = params[0];
	return exp((-5*t)/t0);
}

static float modulacion_quartcos(double t, float params[3]){
	float t0 = params[0];
	return cos((PI*t) / (2*t0)); 
}

static float modulacion_quartsin(double t, float params[3]){
	float t0 = params[0];
	return sin((PI*t) / (2*t0));
}

static float modulacion_halfcos(double t, float params[3]){
	float t0 = params[0];
	return (1+cos(PI*t / t0)) / 2;
}

static float modulacion_halfsin(double t, float params[3]){
	float t0 = params[0];
	return (1+sin(PI*(t/t0 - 1/2))) / 2;
}

static float modulacion_log(double t, float params[3]){
	float t0 = params[0];
	return log10((9*t) / t0 + 1);
}

static float modulacion_invlog(double t, float params[3]){
	float t0 = params[0];
	if(t < t0)
		return log10((-9*t) / t0 + 10);
	else return 0;
}

static float modulacion_tri(double t, float params[3]){
	float t0 = params[0], t1 = params[1], a = params[2];
	if(t < t1)
		return (t*a) / t1;
	else
		return ((t-t1)/(t1-t0));
}

static float modulo(float x){
	if(x<0)
		return x * (-1);
	else return x;
}

static float modulacion_pulses(double t, float params[3]){
	float t0 = params[0], t1 = params[1], a = params[2];
	float t_prim = (t/t0 - (int)t/t0) * t0;
	float valor= modulo(((1-a)/t1)*(t_prim-t0+t1)) + a;
	return floor(valor);
}

//devuelve la cantidad de armonicos que se deben leer del archivo.txt que recibe
bool leer_num_armonicos(FILE *f, size_t *n_fa){
	char cadena[MAX_CADENA];
	if(fgets(cadena, MAX_CADENA, f) == NULL)
		return false;

	*n_fa = atoi(cadena);
	return true; 	
}

//lee los armonicos del archivo.txt que recibe y los guarda en una matriz de flotantes
bool leer_armonicos(FILE *f, float fa[][2], size_t n_fa){
	for(size_t i=0; i < n_fa;i++){
		char cadena[MAX_CADENA];
		if(fgets(cadena, MAX_CADENA,f) == NULL)
			return false;
		size_t j;
		for(j=0; isspace(cadena[j]);j++);

		fa[i][0] = atof(cadena);
		fa[i][1] = atof(cadena+j+1);
	}

	return true;
}								

//arreglo de cadena de caracteres para poder identificar del archivo.txt la funcion que va a realizar el ataque, sostenido o decaimiento de las notas 
static char *nombres_funciones[14]={"CONSTANT", "LINEAR", "INVLINEAR", "EXP", "INVEXP", "QUARTCOS", "QUARTSIN", "HALFCOS",
								"HALFSIN", "LOG", "INVLOG", "SIN", "TRI", "PULSES"};

//arreglo de punteros a funcion de las funciones de modulacion
static float (*funcion[14])(double, float[3]) = {modulacion_constante, modulacion_linear, modulacion_invlinear, modulacion_exp, modulacion_invexp,
										 modulacion_quartcos, modulacion_quartsin, modulacion_halfcos, modulacion_halfsin, modulacion_log, 
										 modulacion_invlog, modulacion_sin, modulacion_tri, modulacion_pulses};

//lee la funcion de ataque, sostenido o decaimiento. Si es valida devuelve true y si es invalida devuelve false
bool pos_funcion(FILE *f, modulacion_t *n){
	char cadena[MAX_CADENA];
	for(size_t i=0; i<MAX_CADENA;i++){
		cadena[i]=fgetc(f);
		if(isspace(cadena[i])){
			cadena[i]='\0';
			break;
		}
	}
	for(size_t i=0; i<14;i++){
		if(!strcmp(cadena, nombres_funciones[i])){
			*n = i;
			return true;
		}
	}
	return false;
}

//lee los parametros de una funcion de modulacion de un archivo.txt en un arreglo de floats y tambien devuelve la funcion de ataque, sostenido o decaimiento
//devuelve true si pudo leer los parametros y false si no pudo leer los parametros o la cantidad de parametros pasados es incorrecta
bool leer_funcion_modulacion(FILE *f, modulacion_t *n, float params[3]){
	if(!pos_funcion(f,n))
		return false;
	char cadena[MAX_CADENA];
	if(*n == 0){
		int a = fgetc(f);
		if(a != '\n')
			return false;
		return true;
	}

	if(fgets(cadena,MAX_CADENA,f) == NULL)
		return false;
	
	if(*n < 12){
		params[0] = atof(cadena);
		return true;
	}
	if(*n == 12){
		size_t i;
		for( i=0; isspace(cadena[i]); i++);
		params[0] = atof(cadena);
		params[1] = atof(cadena+i+1);	
	}
	size_t i=0;
	while(!isspace(cadena[i]))
		i++;
	params[0] = atof(cadena);
	i++;
	params[1] = atof(cadena+i);
	while(!isspace(cadena[i]))
		i++;
	i++;
	params[2] = atof(cadena+i);
	return true;
}

//dada la informacion de una nota, la frecuecia de muestreo y los armonicos devuelve tramo con valores de una onda no sintetizada
tramo_t *crear_onda_nota(datos_notas_t nota, int f_m, float fa[][2], size_t n_fa){
    float f = frecuencia_nota(nota.nota, nota.octava);
    tramo_t *t = tramo_crear_muestreo(nota.t0, nota.tf, f_m, f, nota.velocidad, fa, n_fa);
    if(t == NULL){
        return NULL;
    }
    return  t;
}


//dado los valores de un tramo sin sintetizar, sintetiza el tramo de acuerdo a la funcion y parametros de ataque, sostenido y decaimiento
bool sintetizar_nota(tramo_t *t, modulacion_t a, float ataque[3], modulacion_t s, float sostenido[3], modulacion_t decay, float decaimiento[3]){
	double d = t->t0 + (double)(t->n - 1)/t->f_m;

	if(! tramo_redimensionar(t, d + decaimiento[0]))
		return false;

	float ta = ataque[0];
	float td = decaimiento[0];

	for(size_t i=0; i < t->n; i++){
		double ti = t->t0 + (double)i/t->f_m;
		if(ti > t->t0 && ti < (t->t0 + ta)){
			t->v[i] *= funcion[a](ti - t->t0, ataque);
		}
		if(ti >= (t->t0 + ta) && ti < (t->t0 + a)){
			t->v[i] *= funcion[s](ti - (t->t0 + ta), sostenido);
		}
		if(ti >= (t->t0 + d) && ti < (t->t0 + d + td)){
			t->v[i] *= funcion[decay](ti-(t->t0 + d), decaimiento) * funcion[s](t->t0 + d, sostenido);
		}
	}
	return true;
}