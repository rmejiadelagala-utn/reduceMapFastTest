/*
 * seudoNodo.c
 *
 *  Created on: 30/6/2015
 *      Author: utnso
 */
#include "seudoNodo.h"


//Ruta donde se encuentra el archivo de configuraciÃ³n

int main(int argc, char** argv) {
	printf("***************HOLA SOY UN PROCESO NODO\n****************");

	t_config *config;
	char* archCfg;
	int servNodo; //socket servidor
	pthread_t thr;

	if (argc != 2){
		printf("Error: Se esperaba un parametro \nUso: ./Nodo seudoNodo.cfg\n");
		return 1;
	}
	archCfg=(char*)malloc(strlen(argv[1]));
	archCfg=argv[1];
	config=config_create(archCfg);
	printf("muestro puertonodo, si puedo, significa que config esta ok\n");
	printf("puerto_nodo: %d\n", config_get_int_value(config, "PUERTO_NODO"));
	if (crearServerMultiHilo(config_get_int_value(config, "PUERTO_NODO"),(void*)conexionJobs) == 0)
		printf("Nodo en la espera de conexiones de JOBs y otros Nodos\n");

	return 0;

}
