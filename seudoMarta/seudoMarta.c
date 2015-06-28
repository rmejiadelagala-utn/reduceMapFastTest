/*
 * MaRTA.c
 *
 *  Created on: 26/4/2015
 *      Author: rmejia
 */

#include "seudoMarta.h"

t_config *config;

int main() {
	//char *c;
	int serv; //socket servidor
	int cliente; //socket cliente del FS
	pthread_t thr;

	config=config_create("../marta.cfg");
	serv = crearServer(config_get_int_value(config, "PORT")); //Jobs
//	cliente = crearCliente(config_get_string_value(config,"IP_FS"),config_get_int_value(config,"PUERTO_FS")); //FS
	pthread_create(&thr, NULL, atencionJobs, (void*) &serv );
	pthread_join(thr,NULL);

	printf("Me voy.\n");
	return 0;
}

