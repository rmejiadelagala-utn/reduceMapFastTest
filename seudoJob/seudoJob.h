/*
 * job.h
 *
 *  Created on: 15/5/2015
 *      Author: rmejia
 */

#ifndef SEUDOJOB_H_
#define SEUDOJOB_H_
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <socketes/servidor.h>
#include <socketes/envioDeMensajes.h>
#include <stdbool.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

//esta estructura también va a estar replicada en MaRTA/atencion_Job.h con lo cual habría que ponerla en socketes/estructuras.h
typedef struct {
	int length;
	char* data;
}t_steam;



typedef char* t_filename;

typedef struct {
	t_filename* archivos;
	t_filename  archivo_resultado;
	uint32_t   combiner;
} t_solicitud;

typedef struct {
	char* IP;
	uint16_t PORT;
	char* MAPPER;
	char* REDUCE;
	char* COMBINER;
	char** archivos;
	char*  archivo_resultado;
} t_config_job;

typedef struct{
	char* MAPPER;
	char* REDUCE;
}t_rutinas;

typedef struct {
	int sockMarta;
	char* pathMapper;
	t_ordenMap* ordenMapper;
	} t_arg_hilo_map;

void crearHiloMapper(int , char*);
void crearHiloReduce(int , char*);
t_config_job* leer_archivo_configuracion(char* arch);
uint32_t cantidadArchivosToProcesar(t_solicitud* sol);
uint32_t tamanioBufferSerializar(t_solicitud* sol);
void serializer_y_send_solicitud(int sock, t_solicitud* solicitud);
void recv_y_respuesta_operaciones(void);
t_solicitud cargarEstructuraSolicitud(t_config_job*);
t_rutinas cargarEstructuraRutinas(t_config_job*);
int procesarOrdenesDeMarta(int , t_rutinas* );
int recibirResultadoFromNodo(int sockNodo);
int responderOrdenMapAMarta(int sockMarta, t_ordenMap ordenMapper,int resOper);
char* subirCodigoDeMapper(char* path);

#endif /* SEUDOJOB_H_ */

