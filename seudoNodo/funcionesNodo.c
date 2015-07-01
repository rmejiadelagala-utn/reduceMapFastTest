/*
 * funcionesNodo.c
 *
 *  Created on: 18/5/2015
 *      Author: utnso
 */
#include "seudoNodo.h"

uint32_t TAMANIODISCO, TAMANIOARCHIVO;
int fd = -1, fd_a;


void crearScriptMapper(const char* codigo_script){

	FILE* scriptMapper;

	if((scriptMapper=fopen("/tmp/mapper.sh","w+"))==NULL){
		perror("Error al crear el script del mapper");
		exit(1);
	}

	fputs(codigo_script,scriptMapper);
	
	char *permisosCommand = string_new();

	string_append(&permisosCommand, "chmod u+x ");
	string_append(&permisosCommand,"/tmp/mapper.sh");

	system(permisosCommand);
	fclose(scriptMapper);

	return;
}

void crearScriptReduce(const char* codigo_script){

//	FILE* fd;
	FILE* scriptReduce;
//	char texto[100];

//	if((fd =fopen("/home/utnso/Escritorio/TPSO-2015/reduce.pl","r"))==NULL){
//		perror("Error al abrir el script del reduce");
//		exit(1);
//	}

//	while (feof(fd) == 0) {
//		fgets(texto,100,fd);
//		printf( "%s",texto );
//	}


	if((scriptReduce=fopen("/tmp/reduce.pl","w+"))==NULL){
		perror("Error al crear el script del Reduce");
		exit(1);
	}

//	while (feof(fd) == 0) {
//		fgets(texto,100,fd);
//		fputs(texto,scriptReduce);
//	}

	fputs(codigo_script,scriptReduce);
	
	char *permisosCommand = string_new();

	string_append(&permisosCommand, "chmod u+x ");
	string_append(&permisosCommand,"/tmp/reduce.pl");

	system(permisosCommand);
	fclose(scriptReduce);

	return;
}

int redireccionar_stdin_stdout(char *pathPrograma,char *pathArchivoSalida,char* data_bloque)
{
	FILE *stdin = NULL;

	char *comando = malloc(strlen(pathPrograma)+11+strlen(pathArchivoSalida));

	sprintf(comando,"%s | sort > %s",pathPrograma, pathArchivoSalida);	

	stdin = popen (comando,"w");

	if (stdin != NULL){

		fprintf(stdin, "%s\n",data_bloque);

		pclose (stdin);
		free(comando);
	}
	else{

		printf("No se pudo ejecutar el programa!");
		return -1;
	}

	return 0;
}

t_solicitudMap deserealizarSolMapper(int sock){
	t_solicitudMap solicitudMap;
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
		return solicitudMap;
}

void ejecutarMapper(char * path_s, char* path_tmp, char* datos_bloque){
	printf("estoy en ejecutar Mapper\n");
	if ((redireccionar_stdin_stdout(path_s, path_tmp, datos_bloque)) < 0)
		printf("Error al ejecutar Mapper");
	return;

}

void ejecutarReduce(char * path_s, char* path_tmp, char* datos_bloque){

	if ((redireccionar_stdin_stdout(path_s, path_tmp, datos_bloque)) < 0)
		printf("Error al ejecutar Reduce");
	return;
}

int recibirSolicitudDeJob(int sock){
	int tipoSolicitud, nbytes;
	t_solicitudMap solMap;
	nbytes=recvall(sock,&tipoSolicitud,sizeof(uint32_t));
	printf("tipoSolicitud=%d\n",tipoSolicitud);
	switch(tipoSolicitud){
    case ORDER_MAP:
        printf("ejecutar mapper\n");
        solMap=deserealizarSolMapper(sock);
        //harcod: para probar agrego este texto que seria el bloque;
        char* bloque="Un nodo que se conecta al sistema puede ser un nodo sini datos (nuevo) o un Nodo\nque ya pertencio al Filesystem y contiene bloques de datos que corresponde a archivos de MDFS.\n";
        bloque[strlen(bloque)]='\0';
        ejecutarMapper(solMap.codigoRutina, solMap.temp_file_name, bloque );
        break;
    case ORDER_REDUCE:
        printf("ejecutar reducer\n");
//            crearHiloReduce(sockMarta,rutinas->REDUCE);
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
