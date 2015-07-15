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

int aparearArchivosDeLaListaArchivosAReducir(t_list* archivosAReducir){
	t_list* Lista_reg; //lista de Registros a aparear
	t_RegistroArch* rowSeleccionado;
	/*Me conecto a los remotos y obtengo los sockets y los primeros registros,
	  Para los archivos locales, obtengo los fileDescriptors */
	Lista_reg=getFirstRowsFromArchivosAReducir(archivosAReducir);
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

