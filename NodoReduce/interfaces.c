/*
 * interfaces.c
 *
 *  Created on: 25/5/2015
 *      Author: utnso
 */
#include "interfaces.h"

#include <socketes/paquetesNodo.h>
    char *DATOS;

void* conexionFS(void* arg){


	uint32_t tam_disco;
	t_hilofs* ptr;
	ptr = (t_hilofs*) arg;
	int socket = ptr->socket;
	printf("Nodo Conectado al Filesystem\n");

	tam_disco = obtener_tamanio_disco(ptr->ARCH_BIN);
	int cant_bloques = tam_disco / BLKSIZE;

	DATOS = mapeo_disco(ptr->ARCH_BIN);

	t_nodoParaFS* infoNodo = malloc(sizeof(t_nodoParaFS));
	infoNodo->IP_NODO.s_addr = inet_addr (ptr->IP_NODO);
	infoNodo->PUERTO_NODO = ptr->PUERTO_NODO;
	infoNodo->NODO_NEW = (ptr->NODO_NEW == 's' || ptr->NODO_NEW == 'S');
	infoNodo->CANT_BLOQUES = cant_bloques;
	infoNodo->ID = ptr->ID;

	presentarseAlFileSystem(infoNodo, socket);
	system("clear");

	uint32_t nrobloque;
	int protocolo;
	int recibido;
	int nroBloqueRecibido;
	int resultado;

	while ((recibido=recvall(ptr->socket,&protocolo,4))>0) {

		switch (protocolo){
		  case SET_BLOQUE:
			  nroBloqueRecibido = setBloqueDeFileSystem(socket, DATOS, BLKSIZE);//Si devuelve 0 es porque recibio todo
			  msync(ptr->ARCH_BIN,strlen(DATOS),MS_SYNC);
			  respuestaSetBloque(socket,nroBloqueRecibido);
			  printf("Almacené la informacion en el bloque numero %d correctamente.\n",nroBloqueRecibido);
			  fflush(stdout);
			  break;

		 case GET_BLOQUE:

			  resultado = getBloqueParaFileSystem(socket,DATOS,BLKSIZE);
			  break;

		 case NODO_ESTAS:
			 resultado=recibirInt(socket);
			 break;

		}


	}

	return 0;
}

void* conexionJobs(void* sockJobNodo){

	pthread_detach(pthread_self());

	int sock_in = *(int*)sockJobNodo;
	int recibido;
	int i;
	int protocolo;
	char* script;
	char* archivoSalida;
	int nroBloque;
	int cantArchivosRecibidos;
	int numeroMapActual;
	int numeroReduceActual;
	t_list* archivosRecibidos;
	char* nombreScript;
	char* nomArchSalida;
	int cantArchReduce;
	t_list* archivosAReducir;
	//para etiqueta ENVIO_ARCHIVOS_NODO_NODO
	t_buffer* buffer;
	char* nombreArchivoSolicitado;
	char* dataAEnviar;

	while((recibido=recvall(sock_in,&protocolo,sizeof(int)))>1){
		switch(protocolo){
		case ORDER_MAP:
			//Dejo que el nodo conteste que todo esta bien por ahora, despues hay que hacer el map aca

			printf("Recibi una orden de map, esta todo OK.\n");
			fflush(stdout);
			script=recibirString(sock_in);
			recvall(sock_in,&nroBloque,sizeof(int));
			archivoSalida=strdup("/tmp/");
			nomArchSalida=recibirString(sock_in);
			string_append(&archivoSalida,nomArchSalida);
			printf("El script recibido es %s\n",script);
			printf("El archivo de salida recibido es %s\n",archivoSalida);
			fflush(stdout);
			pthread_mutex_lock(&numeroMap);
			nombreScript = strdup("tmp/mapper");
			string_append(&nombreScript,string_itoa(numeroDeMap));
			string_append(&nombreScript,".sh");
			numeroMapActual = numeroDeMap;
			crearScriptMapper(script,nombreScript);
			numeroDeMap++;
			pthread_mutex_unlock(&numeroMap);
			char* dataAUX = malloc(strlen(DATOS+(nroBloque*BLKSIZE))-1);
			memcpy(dataAUX,DATOS+(nroBloque*BLKSIZE),strlen(DATOS+(nroBloque*BLKSIZE))-1);
			ejecutarMapper(nombreScript,archivoSalida,dataAUX);
			free(script);
			free(dataAUX);
			free(nombreScript);
			free(nomArchSalida);
			free(archivoSalida);
			protocolo=RES_MAP;
			sendall(sock_in,&protocolo,sizeof(int));
			protocolo=OK_MAP; //TODO responder NOTOOK_MAP si hubo algun error
			sendall(sock_in,&protocolo,sizeof(int));
			pthread_exit(NULL);
			break;

		case ENVIO_ARCHIVOS_NODO_NODO:
			// Recibo solicitud de archivoLocal de otro Nodo.

			buffer=(t_buffer*)malloc(sizeof(t_buffer));
			nombreArchivoSolicitado=recibirString(sock_in);
			dataAEnviar=subirArchivoTmpABuffer(nombreArchivoSolicitado);
			buffer->data=strdup(dataAEnviar);
			buffer->tamanio=(strlen(dataAEnviar)+1);
			enviarBuffer(buffer,sock_in);
			break;
/*
			archivosRecibidos = list_create();
			cantArchivosRecibidos = recibirInt(sock_in);
			for(i=0;i<cantArchivosRecibidos;i++){
				guardarEnDisco(recibirArchivo(sock_in));
			}
			break;
*/
		case ORDER_REDUCE:
			archivosAReducir=list_create();
			script=recibirString(sock_in);
			cantArchReduce=recibirInt(sock_in);
		    for(i=0;i < cantArchReduce;i++){
		    	list_add(archivosAReducir,nuevoArchReduce(recibirInt(sock_in),recibirInt(sock_in),recibirString(sock_in)));
		    }
		    nomArchSalida=recibirString(sock_in);
		    printf("El script recibido es %s\n",script);
		    printf("El archivo de salida recibido es %s\n",archivoSalida);
		    fflush(stdout);
		    pthread_mutex_lock(&numeroReduce);
		    nombreScript = strdup("tmp/reduce");
		    string_append(&nombreScript,string_itoa(numeroDeReduce));
		    string_append(&nombreScript,".sh");
		    numeroReduceActual = numeroDeReduce;
		    crearScriptReduce(script,nombreScript);
		    numeroDeReduce++;
		    pthread_mutex_unlock(&numeroReduce);
		    //APAREAR ARCHIVOS DE LA LISTA ARCHIVOSAREDUCIR
		    aparearArchivosDeLaListaArchivosAReducir(archivosAReducir,nombreScript,archivoSalida);
		}
	}


	return 0;
}



t_archivoAReducir* nuevoArchReduce(int ip, int puerto, char* nombre){
	t_archivoAReducir* unArch = malloc(sizeof(t_archivoAReducir));
	unArch->ipNodo=ip;
	unArch->puertoNodo=puerto;
	unArch->nombreArch=strdup(nombre);
	return unArch;
}
