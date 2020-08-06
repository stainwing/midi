#include "formato.h"
#include "evento.h"
#include "nota.h"
#include "lectura_midi.h"
#include "notas_midi.h"

#define METAEVENTO_FIN_DE_PISTA 0x2F
#define EVENTO_MAX_LONG 10

enum {EVNOTA_NOTA, EVNOTA_VELOCIDAD};
enum {METAEVENTO_TIPO, METAEVENTO_LONGITUD};

//Recibe un archivo.mid y extrae la informacion de las notas del canal indicado en los parametros de la funcion
//devuelve la cantidad de notas leidas y un arreglo de datos_notas_t donde se almacena la informacion de las notas
//el arreglo debe 
bool leer_midi(FILE* f, char canal_recibido, int pps, datos_notas_t *notas, size_t n_notas ,size_t *notas_leidas){
    // LECTURA DEL ENCABEZADO:
    formato_t formato;
    uint16_t numero_pistas;
    uint16_t pulsos_negra;

    if(! leer_encabezado(f, &formato, &numero_pistas, &pulsos_negra)) {
        printf("fallo al leer encabezado\n");
        return false;
    }
    //cambiar pulsos por negra a pulsos por segundo
    size_t i=0;
    // ITERAMOS LAS PISTAS:
    for(uint16_t pista = 0; pista < numero_pistas; pista++) {
        // LECTURA ENCABEZADO DE LA PISTA:
        uint32_t tamagno_pista;
        if(! leer_pista(f, &tamagno_pista)) {
            printf("fallo al leer pista\n");
            return false;
        }

        evento_t evento;
        char canal_leido;
        int longitud;
        uint32_t tiempo = 0;

        // ITERAMOS LOS EVENTOS:
        while(1) {
            uint32_t delta_tiempo;
            leer_tiempo(f, &delta_tiempo);
            tiempo += delta_tiempo;

            // LECTURA DEL EVENTO:
            uint8_t buffer[EVENTO_MAX_LONG];
            if(! leer_evento(f, &evento, &canal_leido, &longitud, buffer)) {
                printf("fallo al leer evento\n");
                return false;
            }

            // PROCESAMOS EL EVENTO: 
            if(evento == EVENTO_METAEVENTO && canal_leido == 0xF) {
                // METAEVENTO:
                if(buffer[METAEVENTO_TIPO] == METAEVENTO_FIN_DE_PISTA) {
                    break;
                }

                descartar_metaevento(f, buffer[METAEVENTO_LONGITUD]);
            }

            else if(canal_leido == canal_recibido){ //valido el canal recibido
                if (evento == EVENTO_NOTA_ENCENDIDA || evento == EVENTO_NOTA_APAGADA) {
                    // NOTA:
                    nota_t nota;
                    signed char octava;
                    if(! decodificar_nota(buffer[EVNOTA_NOTA], &nota, &octava)) {
                        printf("fallo al leer nota\n");
                        return false;
                    }
                    if(evento == EVENTO_NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] != 0){
                        if(i == n_notas)
                            return false;
                        //guardamos la informacion de la nota
                        notas[i].nota = nota;
                        notas[i].octava = octava;
                        notas[i].t0 = tiempo / (double)pps; //convertimos los pulsos por negra a pulsos por segundo
                        notas[i].tf = 0;
                        notas[i].velocidad = buffer[EVNOTA_VELOCIDAD];
                        i++;
                    }
                    if(evento == EVENTO_NOTA_APAGADA || (evento == EVENTO_NOTA_ENCENDIDA && buffer[EVNOTA_VELOCIDAD] == 0)){
                        for(size_t j=0; j < i ; j++){
                            if((notas[j].nota == nota) && (notas[j].octava == octava) && (notas[j].tf == 0)){
                                notas[j].tf = tiempo / (double)pps; //guardamos el tiempo de finalizacion de una nota
                            }
                        }
                    }
                }
            }    
        }
    }
    *notas_leidas = i;
    return true;
}

