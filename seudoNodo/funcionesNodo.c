/*
 * funcionesNodo.c
 *
 *  Created on: 18/5/2015
 *      Author: utnso
 */
#include "seudoNodo.h"
#include <string.h>

uint32_t TAMANIODISCO, TAMANIOARCHIVO;
int fd = -1, fd_a;

int  responderResultadoAJob(int sockJob,int resultMap){
	 t_buffer* buffer = crearBufferConProtocolo(RES_MAP);
	 bufferAgregarInt(buffer,resultMap);
	 int resultado = enviarBuffer(buffer,sockJob);
     return resultado;
}


char* crearScriptMapper(const char* codigo_script){

	FILE* scriptMapper;
	/* harcode el nombre de la rutina, luego debo ver como generarlo
	 * con un date al final, para diferenciarlo de otros mappers de
	 * otros jobs que puedan correr en paralelo
	 */
	char*pathRutina="/tmp/mapper.sh";

	if((scriptMapper=fopen(pathRutina,"w+"))==NULL){
		perror("Error al crear el script del mapper");
		exit(1);
	}

	fputs(codigo_script,scriptMapper);
	
	char *permisosCommand = string_new();

	string_append(&permisosCommand, "chmod a+x ");
	string_append(&permisosCommand,"/tmp/mapper.sh");

	system(permisosCommand);
	fclose(scriptMapper);

	return pathRutina;
}

char* crearScriptReduce(const char* codigo_script){
	FILE* scriptReduce;
	char* pathScript=(char*)malloc(40);
	pathScript="/tmp/reduce.pl";

	if((scriptReduce=fopen(pathScript,"w+"))==NULL){
		perror("Error al crear el script del Reduce");
		exit(1);
	}

	fputs(codigo_script,scriptReduce);
	
	char *permisosCommand = string_new();

	string_append(&permisosCommand, "chmod a+x ");
	string_append(&permisosCommand,pathScript);

	system(permisosCommand);
	free(permisosCommand);
	fclose(scriptReduce);

	return pathScript;
}



t_solicitudMap deserealizarSolMapper(int sock){
	t_solicitudMap solicitudMap;
	printf("ingrese a funcion deserealizar\n");
		int nroBloque,tam;
		char* codigoMap, arch_tmp;
		recvall(sock,&tam,sizeof(uint32_t));
		solicitudMap.codigoRutina=(char*)malloc(tam+1);
		recvall(sock,solicitudMap.codigoRutina,tam);
		solicitudMap.codigoRutina[tam]='\0';
		recvall(sock,&solicitudMap.block,sizeof(uint32_t));
		recvall(sock,&tam,sizeof(uint32_t));
		solicitudMap.temp_file_name=(char*)malloc(tam+1);
		recvall(sock,solicitudMap.temp_file_name,tam);
		solicitudMap.temp_file_name[tam]='\0';
		printf("%s\n",solicitudMap.temp_file_name);
		printf("saliendo de funcion deserealizar\n");
		return solicitudMap;
}

int ejecutarMapper(char * path_s, char* path_tmp, char* datos_bloque){
	printf("estoy en ejecutar Mapper\n");
	if ((redirec_stdin_stdout(path_s, path_tmp, datos_bloque)) < 0){
		return -1;
	}
	return 0;

}

int ejecutarReduce(t_solicitudRed solRed){
	char* pathRutina;
	pathRutina=crearScripReduce(solRed.codigoRutina);
//	if ((redirec_stdin_stdout(path_s, path_tmp, datos_bloque)) < 0){0
//		printf("Error al ejecutar Reduce");
//	}
	return 0;
}

int recibirSolicitudDeJob(int sock){
	int tipoSolicitud, nbytes,lenBloque;
	t_solicitudMap solMap;
	t_solicitudRed solRed;
	nbytes=recvall(sock,&tipoSolicitud,sizeof(uint32_t));
	char* pathRutina;
	printf("tipoSolicitud=%d\n",tipoSolicitud);
	switch(tipoSolicitud){
    case ORDER_MAP:
        printf("ejecutar mapper\n");
        solMap=deserealizarSolMapper(sock);
        //harcod: para probar agrego este texto que seria el bloque;
		printf("muestro en nodo codigo de rutina mapper\n");
		printf("%s\n",solMap.codigoRutina);
		printf("muestro bloque que mando para mapear:\n");
		lenBloque=strlen("Un nodo que se conecta al sistema puede ser un nodo sini datos (nuevo) o un Nodo\nque ya pertencio al Filesystem y contiene bloques de datos que corresponde a archivos de MDFS.\n");
        char* bloque=(char*)malloc(lenBloque);
        strcpy(bloque,"Un nodo que se conecta al sistema puede ser un nodo sini datos (nuevo) o un Nodo\nque ya pertencio al Filesystem y contiene bloques de datos que corresponde a archivos de MDFS.\n");
        lenBloque=strlen(bloque);
        bloque[lenBloque]='\0';
		printf("%s\n",bloque);
		pathRutina=crearScriptMapper(solMap.codigoRutina);
        if(ejecutarMapper(pathRutina, solMap.temp_file_name, bloque )<0){
        	printf("error al ejecutar Mapper\n");
        	responderResultadoAJob(sock,NOTOK_MAP);
        	return -1;
        } else {
        	printf("termino ok el mapper\n");
        	responderResultadoAJob(sock,OK_MAP);
        }
        free(pathRutina);
        free(bloque);
        break;
    case ORDER_REDUCE:
        printf("ejecutar reducer\n");
        solRed=deserealizarSolReduce(sock);
        if(ejecutarReduce(solRed)<0){
        	printf("error al ejecutar Reduce\n");
        	responderResultadoAJob(sock,NOTOK_REDUCE);
        	return -1;
        } else {
        	printf("termino ok el Reduce\n");
        	responderResultadoAJob(sock,OK_REDUCE);
        }
        free(pathRutina);
        break;
    default:
        printf("error: no se entiende la solicitud\n");
        }
	return 0;
}

int conexionJobs (int* sock){
	int nbytes, protocolo,tipoSolicitud; //Mapper o Reduce
	printf("estoy en conexionJobs\n");
	printf("recibi el sock %d\n",*sock);
	int sockJob=*sock;
	if ((nbytes=recvall(sockJob,&protocolo,sizeof(uint32_t)))>0){
		if(protocolo==CONEXION_JOB_A_NODO){
			printf("Tengo un Job conectado, atendido por sock: %d\n",sockJob);
			recibirSolicitudDeJob(sockJob);
		} else {
			printf("No entiendo el protocolo, cierro conexion");
			close(sockJob);
		}
	} else {
		printf("error al recibir conexion de job");
		perror(recvall);
	}
	return 0;
}
