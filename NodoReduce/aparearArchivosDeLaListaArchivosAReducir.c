/*
 * aparearArchivosDeLaListaArchivosAReducir.c
 *
 *  Created on: 14/7/2015
 *      Author: rmejia
 */
#include "nodo.h"
#include "interfaces.h"
#include "estructurasNodo.h"
#include "funcionesNodo.h"


t_list* getFirstRowsFromArchivosAReducir(t_archivoAReducir* unArchivoAReducir ){
	t_list* Lista_reg; //lista de Registros a aparear
	t_RegistroArch* unRegistroArch;
	Lista_reg=list_create();
    if(esLocal(unArchivoAReducir)){
    	unRegistroArch=getRegistroDeArchivo()
    }
}

int aparearArchivosDeLaListaArchivosAReducir(t_list* archivosAReducir){
	t_list* Lista_reg; //lista de Registros a aparear
	t_RegistroArch* rowSeleccionado;
	/*Me conecto a los remotos y obtengo los sockets y los primeros registros,
	  Para los archivos locales, obtengo los fileDescriptors */
	Lista_reg=list_map(archivosAReducir,(void*)getFirstRowsFromArchivosAReducir);
	while( !list_is_empty(Lista_reg)){
		rowSeleccionado=apareoDeRegistros(Lista_reg);
		enviarRowAlPipeDelReduce(rowSeleccionado->registro);
		/* Tengo que actulizar la lista de registros, es como avanzar
		 * el puntero. Uso el rowSeleccionado para obtener el sock y
		   poder conectarme al nodo remotor*/
		actualizarListaDeRegistros(Lista_reg,rowSeleccionado);
		//en la misma funcion anterior debo controlar el EOF
	}
	return 0;
}

