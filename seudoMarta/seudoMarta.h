/*
 * MaRTA.h
 *
 *  Created on: 22/6/2015
 *      Author: utnso
 */

#ifndef SEUDOMARTA_H_
#define SEUDOMARTA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/config.h>
#include<socketes/servidor.h>
#include <pthread.h>
#include <socketes/envioDeMensajes.h>
#include <commons/collections/list.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>

typedef char* t_filename;


typedef struct {
	int numeroDeBloqueArchivo;
	int mapOReduce;
	int estado;
	t_list *copiasDelBloque; //serÃ­an de t_bloqueEnNodo
} t_planificacion; //esta la genera con el algoritmo de planificar las tareas

typedef struct {
	char *nombreArchivo;
	t_list *planificacion; //serÃ­an de t_planificacion
} t_planificacionPorCadaArchivo;


typedef struct {
	int id_Job;	//Va a ser otra cosa. Solo es la idea
	char *nombreArchivoResultado;
	int combinerONO;
	int estado;
	t_list *planificacionesArchivo; //serÃ­an de t_planificacionPorCadaArchivo
} t_jobsAPlanificar;

////////// Info pedidad a FS /////////////
typedef struct{
	int numeroDeBloque;
	t_list *bloquesEnNodo; //serÃ­an t_bloqueEnNodo
} t_bloqueDeArchivo;


typedef struct {
	char *nombreArchivo;
	t_list *bloquesDeArch; //serÃ­an t_bloqueDeArchivo
} t_infoDeFileSystem;

///////// Info de carga de Nodos en el Sistema /////////
typedef struct {
	char *ipPuerto;
	int carga;
	int estado;
} t_infoNodo;

/////////////Nuevas estructuras de Marta que surgen del algorit. de distribuir y manda mensajes////////////
//////////// Estructuras de MaRTA Para envio de mesajes y demÃ¡s /////////////////////////
typedef struct {
	int idJob;
	int combiner;
	char* pathDeResultado;
	char** pathsDeArchivos;
} t_InfoJob;

typedef struct {
	int idArchivo;
	int cantidadDeBloques;
} t_InfoArchivo;

typedef struct {
	uint32_t id_map;
	uint32_t id_nodo;
//	char* ip_nodo[16];
	uint32_t ip_nodo;
	uint32_t puerto_nodo;
	uint32_t block;
	char* temp_file_name;
} t_DestinoMap;

typedef struct {
	t_InfoArchivo* file;
	uint32_t block;
	t_DestinoMap* map_dest;
} t_MapPendiente;

typedef struct {
	uint32_t prot;
	uint32_t id_map;
} t_ResultadoMap;

typedef struct {
	uint32_t id_temp;
	char* path;
	uint32_t id_nodo;
	uint32_t id_file_origin;
	uint32_t block_origin;
} t_MapTemporal; //Revisar bien esta

///Esto va en envioDeMensajes me parece///

typedef struct {
	uint32_t id_nodo;
	uint32_t ip_nodo;
	uint32_t puerto_nodo;
	uint32_t block;
} t_CopiaDeBloque;

typedef struct {
	uint32_t id_nodo;
	uint32_t cantidadOperacionesEnCurso;
} t_CargaNodo;

typedef struct {
	t_filename* archivos;
	t_filename  archivo_resultado;
	uint32_t   combiner;
} t_solicitud;


t_list *listaRegistrosIDIP;
void* atencionJobs(void*);

#endif /* SEUDOMARTA_H_ */
