/*
 * creacionDeHilos.c
 *
 *  Created on: 24/6/2015
 *      Author: rmejia
 */

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<commons/config.h>
#include<socketes/servidor.h>
#include<socketes/envioDeMensajes.h>
#include<stdbool.h>
#include <pthread.h>
#include "seudoJob.h"

int recibirResultadoFromNodo(int sockNodo){
		uint32_t recibido, protocolo, rptaNodoAJob;
		if((recibido=recvall(sockNodo,&protocolo,sizeof(uint32_t)))<0){
			return -1;
		}
		if(protocolo==RES_MAP || protocolo==RES_REDUCE){
			recvall(sockNodo,&rptaNodoAJob,sizeof(uint32_t));
		}else {
			printf("no entiendo el protocolo, usa: RES_MAP");
		}
		return rptaNodoAJob;

	}

int responderResultOrdenAMarta(int sockMarta,int id_operacion, int resOper){
	int result_envio;
	t_buffer* buffer = crearBuffer();//crearBufferConProtocolo(RES_MAP);
	//Pero tengo que usar algun protocolo para que marta sepa si estoy
	//respondiendo el resultado de un maper o reduce --- FALTA!!!!! ---
	bufferAgregarInt(buffer,resOper);
	bufferAgregarInt(buffer,id_operacion);
	result_envio=enviarBuffer(buffer,sockMarta);
	return result_envio;
}

void* hilo_mapper (void* arg_thread){
	t_arg_hilo_map ordenToNodo;
	t_ordenMap ordenMapper;
	ordenToNodo= *((t_arg_hilo_map*)arg_thread);
	int sockMarta,sockNodo, puerto_nodo,block,res,envioRes;
	int resOper; //resultado de la operación de mapper
	char* tmp_file_name;
	char* codigoMapper;
	char* ip_nodo_char;
	sockMarta=ordenToNodo.sockMarta;
	printf("El path del codigo mapper es %s\n",ordenToNodo.pathMapper);
	codigoMapper=subirCodigoFromPathToBuffer(ordenToNodo.pathMapper);
	ordenMapper=*(ordenToNodo.ordenMapper);
	tmp_file_name=strdup(ordenMapper.temp_file_name);
	block=ordenMapper.block;
	puerto_nodo=ordenMapper.puerto_nodo;
	struct in_addr addr;
	addr.s_addr=ordenMapper.ip_nodo;
	ip_nodo_char= inet_ntoa(addr);
	sockNodo= crearCliente(ip_nodo_char,puerto_nodo);
	//Enviamos rutina mapper a Nodo
	fflush(stdout);
	t_buffer* buffer = crearBufferConProtocolo(CONEXION_JOB_A_NODO);
	enviarBuffer(buffer,sockNodo);
	//bufferAgregarInt(buffer,ORDER_MAP);
	res=enviarMapperANodo(sockNodo,codigoMapper,block,tmp_file_name);
	if(res<0){
		printf("todo mal, no pude enviar mapper a Nodo: %d", ip_nodo_char);
		exit(-1);
	}
	//recibir resultado de la Operacion mapper desde el Nodo
	resOper=recibirResultadoFromNodo(sockNodo);
	if(resOper==OK_MAP){
		printf("mapper id= %d, terminó OK, aviso a Marta\n",ordenMapper.id_map);
	} else {
		printf("mapper id= %d, falló, aviso a Marta de esto\n",ordenMapper.id_map);
	}
	//re-enviar a marta resultado de la operacion, recibida de Nodo
	envioRes=responderResultOrdenAMarta(sockMarta,ordenMapper.id_map,resOper);
	if(envioRes<0){
		printf("no pude enviar la respuesta a marta, algo pasó\n");
	}
	return NULL;
}

void crearHiloMapper(int sockMarta, char* pathMapper) {
	pthread_t thread_map;
	t_ordenMap *ordenMapper;
	t_arg_hilo_map* arg_thread;
	ordenMapper=recibirOrdenMapDeMarta(sockMarta);
	//Muestro lo que recibo de marta
	printf("Muestro lo que recibo de marta\n");
	printf("id_map:%d\n",ordenMapper->id_map);
	printf("id_nodo:%d\n",ordenMapper->id_nodo);
	printf("ip_nodo:%d\n",ordenMapper->ip_nodo);
	struct in_addr addr;
	addr.s_addr=ordenMapper->ip_nodo;
	printf("muestro ip en formato de numeros y puntos: %s\n",inet_ntoa(addr));
	printf("puerto_nodo:%d\n",ordenMapper->puerto_nodo);
	printf("tmp_arch: %s\n",ordenMapper->temp_file_name);
	arg_thread=(t_arg_hilo_map*)malloc(sizeof(t_arg_hilo_map));
	arg_thread->sockMarta=sockMarta;
	arg_thread->pathMapper=strdup(pathMapper);
	int tamanioDeLaOrden = sizeof(t_ordenMap) + strlen(ordenMapper->temp_file_name);
	arg_thread->ordenMapper=malloc(tamanioDeLaOrden);
	arg_thread->ordenMapper=ordenMapper;

	pthread_create (&thread_map, NULL, hilo_mapper, (void*)arg_thread);
	return ;
}


/**********************************************************
 *
 *                   PARTE DEL REDUCE
 *
 **********************************************************/

t_ordenReduce* recibirOrdenReduceDeMarta(int sockMarta){
		t_ordenReduce* ordenReduce = malloc(sizeof(t_ordenReduce));
		uint32_t cantArchAreducir;
		recvall(sockMarta,&(ordenReduce->id_reduce),sizeof(uint32_t));
		recvall(sockMarta,&(ordenReduce->ip_nodo),sizeof(uint32_t));
		recvall(sockMarta,&(ordenReduce->puerto_nodo),sizeof(uint32_t));
		recvall(sockMarta,&(ordenReduce->cantArchAreducir),sizeof(uint32_t));
		//reservo la cantidad necesaria de punteros t_nodoArchTmp
		ordenReduce->nodosArchTmp=(t_nodoArchTmp*)malloc(sizeof(t_nodoArchTmp*)*(ordenReduce->cantArchAreducir));
		int i=0;
		uint32_t tamanioArch=0;
		while (i < (ordenReduce->cantArchAreducir)) {
			recvall(sockMarta,&(ordenReduce->nodosArchTmp[i]->ip_nodo),sizeof(uint32_t));
			recvall(sockMarta,&(ordenReduce->nodosArchTmp[i]->puerto_nodo),sizeof(uint32_t));
			recvall(sockMarta,&(tamanioArch),sizeof(uint32_t));
			(ordenReduce->nodosArchTmp[i]->archTmp)=(char*)malloc(tamanioArch+1);
			recvall(sockMarta,(ordenReduce->nodosArchTmp[i]->archTmp),tamanioArch);
			i++;
		}
		recvall(sockMarta,&(tamanioArch),sizeof(uint32_t));
		(ordenReduce->archResultado)=(char*)malloc(tamanioArch+1);
		recvall(sockMarta,(ordenReduce->archResultado),tamanioArch);
		return ordenReduce;
	}


int enviarReduceANodo(int sockNodo,char* codigoReduce, int cantArchivos,
					  t_nodoArchTmp** nodosArchTmp,char* archResultado){
	t_buffer* buffer = crearBufferConProtocolo(ORDER_REDUCE);
    bufferAgregarString(buffer,codigoReduce,strlen(codigoReduce)+1);
    bufferAgregarInt(buffer,cantArchivos);
    int i=0;
    while(i < cantArchivos){
    	bufferAgregarInt(buffer,nodosArchTmp[i]->ip_nodo);
    	bufferAgregarInt(buffer,nodosArchTmp[i]->puerto_nodo);
    	bufferAgregarString(buffer,nodosArchTmp[i]->archTmp,strlen(nodosArchTmp[i]->archTmp)+1);
    }
    bufferAgregarString(buffer,archResultado,strlen(archResultado)+1);
	int resultado = enviarBuffer(buffer,sockNodo);
	return resultado;
}


void* hilo_reduce (void* arg_thread){
	t_arg_hilo_reduce ordenToNodo;
	t_ordenReduce ordenReduce;
	ordenToNodo= *((t_arg_hilo_reduce*)arg_thread);//copio contenido de arg_thread a ordenToNodo
	int sockMarta,sockNodo, puerto_nodo,res,envioRes;
	int resOper; //resultado de la operación de reduce
	char* archResultado;
	char* codigoReduce;
	char* ip_nodo_char;
	sockMarta=ordenToNodo.sockMarta;
	printf("El path del codigo reduce es %s\n",ordenToNodo.pathReduce);
	codigoReduce=subirCodigoFromPathToBuffer(ordenToNodo.pathReduce);
	ordenReduce=*(ordenToNodo.ordenReduce);
	archResultado=strdup(ordenReduce.archResultado);
	puerto_nodo=ordenReduce.puerto_nodo;
	struct in_addr addr;
	addr.s_addr=ordenReduce.ip_nodo;
	ip_nodo_char= inet_ntoa(addr);
	sockNodo= crearCliente(ip_nodo_char,puerto_nodo);
	//Enviamos rutina reduce a Nodo
	fflush(stdout);
	t_buffer* buffer = crearBufferConProtocolo(CONEXION_JOB_A_NODO);
	enviarBuffer(buffer,sockNodo);
	//bufferAgregarInt(buffer,ORDER_MAP);
	res=enviarReduceANodo(sockNodo,codigoReduce,ordenReduce.cantArchAreducir,ordenReduce.nodosArchTmp,archResultado);
	if(res<0){
		printf("todo mal, no pude enviar reduce a Nodo: %d", ip_nodo_char);
		exit(-1);
	}
	//recibir resultado de la Operacion reduce desde el Nodo
	resOper=recibirResultadoFromNodo(sockNodo);
	if(resOper==OK_REDUCE){
		printf("reduce id= %d, terminó OK, aviso a Marta\n",ordenReduce.id_reduce);
	} else {
		printf("reduce id= %d, falló, aviso a Marta de esto\n",ordenReduce.id_reduce);
	}
	//re-enviar a marta resultado de la operacion, recibida de Nodo
	envioRes=responderResultOrdenAMarta(sockMarta,ordenReduce.id_reduce,resOper);
	if(envioRes<0){
		printf("no pude enviar la respuesta a marta, algo pasó\n");
	}
	free(codigoReduce);
	return NULL;
}


void crearHiloReduce(int sockMarta, char* pathReduce) {
	pthread_t thread_reduce;
	t_ordenReduce* ordenReduce;
	t_arg_hilo_reduce* arg_thread=(t_arg_hilo_reduce*)malloc(sizeof(t_arg_hilo_reduce));
	ordenReduce=recibirOrdenReduceDeMarta(sockMarta);
	arg_thread->sockMarta=sockMarta;
	arg_thread->pathReduce=strdup(pathReduce);
	arg_thread->ordenReduce=ordenReduce;
	pthread_create (&thread_reduce, NULL, &hilo_reduce,(void*)arg_thread);
	//free(ordenReduce); No puedo liberar aca sino me va a liberar antes de
	//free(arg_thread);  que termine el hilo, muy malo!!!!
	return;
}
