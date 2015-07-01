/*
 * seudoNodo.h
 *
 *  Created on: 30/6/2015
 *      Author: utnso
 */

#ifndef SEUDONODO_H_
#define SEUDONODO_H_

#include <stdio.h>
#include <commons/config.h>
#include <commons/string.h>
#include <socketes/servidor.h>
#include <socketes/servidorMultihilo.h>
#include <socketes/envioDeMensajes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <err.h>
#include <time.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>

typedef struct {
	char* IP_FS;
	uint16_t PUERTO_FS;
	char* ARCH_BIN;
	char* DIR_TEMP;
	char NODO_NEW;
	char* IP_NODO;
	uint16_t PUERTO_NODO;
	int ID;
} t_config_nodo;

typedef struct {
	char* codigoRutina;
 	uint32_t block;
 	char* temp_file_name;
 } t_solicitudMap;

int conexionJobs(int sock);
int recibirSolicitudDeJob(int sock);
t_solicitudMap deserealizarSolMapper(int sock);

#endif /* SEUDONODO_H_ */
