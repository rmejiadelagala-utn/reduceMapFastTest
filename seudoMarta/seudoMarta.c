/*
 * MaRTA.c
 *
 *  Created on: 26/4/2015
 *      Author: rmejia
 */

#include "seudoMarta.h"

int main(int argc, char *argv[]) {

	t_config *config;
	char* c;
	int serv; //socket servidor
	pthread_t thr;

	if (argc != 2){
		printf("Error: Se esperaba un parametro \nUso: ./Marta config.cfg\n");
		return 1;
	}
	c=(char*)malloc(strlen(argv[1]));
	c=argv[1];
	config=config_create(c);
	serv = crearServer(config_get_int_value(config, "PORT")); //Jobs
//	cliente = crearCliente(config_get_string_value(config,"IP_FS"),config_get_int_value(config,"PUERTO_FS")); //FS
	pthread_create(&thr, NULL, atencionJobs, (void*) &serv );
	pthread_join(thr,NULL);

	printf("Me voy.\n");
	return 0;
}

