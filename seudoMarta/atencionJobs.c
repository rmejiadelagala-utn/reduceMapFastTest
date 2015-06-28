/*
 * atencionJobs.c
 *
 *  Created on: 17/5/2015
 *      Author: utnso
 */

#include"seudoMarta.h"
#include <socketes/envioDeMensajes.h>
typedef char* t_filename;

int ordenarMapAJob(int socketJob) {

	int result;
	t_DestinoMap* destinoMap;

	//valores HARDCOD para poder testear
	destinoMap->id_map=5;
	destinoMap->id_nodo=2;
//	destinoMap->ip_nodo=strdup("127.0.0.1");
	inet_aton("127.0.0.1",destinoMap->ip_nodo);
	destinoMap->puerto_nodo=5003;
	destinoMap->temp_file_name=strdup("salida26062015.tmp");
	//mostrar_map_dest(destinoDeMap);

	t_buffer* map_order = crearBufferConProtocolo(ORDER_MAP);

	bufferAgregarInt(map_order, destinoMap->id_map);
	bufferAgregarInt(map_order, destinoMap->id_nodo);
	bufferAgregarString(map_order, destinoMap->ip_nodo);
	bufferAgregarInt(map_order, destinoMap->puerto_nodo);
	bufferAgregarInt(map_order, destinoMap->block);
	buffer_add_string(map_order, destinoMap->temp_file_name);
	result = enviarBuffer(socketJob, map_order);
	if (result < 0) {
		printf("No se Pudo enviar la Orden de Map al Job");
	}
	return result;
}


t_solicitud deserealizar(char* buffer, int sockCliente) {
	char* payload;
	int nbytes;
	uint32_t tam_payload;
	t_solicitud solicitud;
	memcpy(&tam_payload,buffer,sizeof(uint32_t));
	printf("tam_payload: %d\n", tam_payload);
	//payload tiene los datos sin el tamaño total
	payload=(char*)malloc(tam_payload);
	if((nbytes=recv(sockCliente,payload,tam_payload,0))<=0) {
        // error o conexión cerrada por el cliente
        if (nbytes == 0) {
            // conexión cerrada
            printf("selectserver: socket %d cerró conexión\n", sockCliente);
        } else {
            perror("recv");
        }
        close(sockCliente); // bye! tal vez aca no tenga que cerrarlo sino en la funcion que me llamó.
	}
	else {
		int i=0;
		size_t long_arch;
		uint32_t cant_arch; //cantidad de archivos a procesar
		int cursor;
		char* bufftmp;
		//el primer campo de payload es cant_arch a procesar
		printf("Mostrare los datos del paquete deserealizados\n");
		memcpy(&cant_arch,payload,sizeof(uint32_t));
		printf("cant_arch: %d\n",cant_arch);
		cursor= 1*sizeof(uint32_t);
		solicitud.archivos = malloc(sizeof(char*)*cant_arch);
		while (i < cant_arch){
			memcpy( &long_arch,(payload+cursor), sizeof(uint32_t));
			cursor+=sizeof(uint32_t);
			bufftmp=malloc(long_arch);
			bufftmp[long_arch]='\0';
			memcpy(bufftmp,payload+cursor,long_arch);
			solicitud.archivos[i]=strdup(bufftmp);
			printf("solicitud_archivo: %s\n", solicitud.archivos[i]);
			cursor+=long_arch;
			i++;
		}
		memcpy(&long_arch ,(payload+cursor),sizeof(uint32_t));
		cursor+=sizeof(uint32_t);
		solicitud.archivo_resultado=malloc(long_arch+1);
		memset(solicitud.archivo_resultado,'\0',long_arch+1);
		memcpy(solicitud.archivo_resultado,(payload + cursor), long_arch );
		cursor+=long_arch;
		memcpy(&solicitud.combiner,payload + cursor,  sizeof(uint32_t));
		printf("%s\n",solicitud.archivo_resultado);
		free(bufftmp);
	}
	return solicitud;
}

void* atencionJobs(void* sock){
	int serv= *(int*)sock;
	int fdmax, i,newSock;
	int sin_size, nbytes;
	printf("estoy en el hilo\n");
	printf("el valor del socket es: %d\n",serv);
	struct sockaddr_in their_addr;
	char* buff;
	//char* prog_socket_str;

	fd_set master;
	fd_set read_fds;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(serv, &master);
	fdmax = serv;
	for (;;) {
		read_fds = master; // copialo
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL ) == -1) {
			perror("select");
			exit(1);
		}
		for(i=0; i<=fdmax; i++){
			if (FD_ISSET(i, &read_fds)) { // tenemos datos!!
				//printf("FD_ISSET %i\n",i);
				if (i == serv) {
					//conexion nueva
					sin_size = sizeof(struct sockaddr_in);
					if ((newSock = accept(serv, (struct sockaddr*) &their_addr,
							(socklen_t *) &sin_size)) < 0) {
						perror("accept");
						exit(1);
					}else {
						FD_SET(newSock, &master); // aniadir al conjunto maestro
						if (newSock > fdmax) {    // actualizar el maximo
							fdmax = newSock;
						}
						printf("nuevo job en %s on socket %d\n", inet_ntoa(their_addr.sin_addr),
									 newSock);
					}
				}
				else {
                    // gestionar datos de un cliente
					t_solicitud solicitud;
					buff=(char*)malloc(sizeof(uint32_t));
                    if ((nbytes = recv(i, buff, sizeof(uint32_t), 0)) <= 0) {
                        // error o conexión cerrada por el cliente
                        if (nbytes == 0) {
                            // conexión cerrada
                            printf("selectserver: socket %d cerró conexión\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // eliminar del conjunto maestro
                    } else {
                        // tenemos datos de algún cliente
                        // ahora lo muestro por stdout pero debo deserealizar y usar la data
                        solicitud=deserealizar(buff,i);
                        ordenarMapAJob(i);
                        recibirResultadoDeOperacion(i);
                        FD_CLR(i, &master); // eliminar del conjunto maestro, seguira la atencion con hilo
                    }
				}
			}
		}
	}
	free(buff);
}
