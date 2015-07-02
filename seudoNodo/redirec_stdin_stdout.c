/*
 * redirec_stdin_stdout.c
 *
 *  Created on: 1/7/2015
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "seudoNodo.h"

/* Esto comentado es solo para probarlo
 void escribirEnArchivo(FILE *archivo)
{
	fprintf (archivo, "boludeces1\nboludeces2\nsalir\n");

}

int main(void) {
	puts("Hola Soy un programa que redirecciona el stdin y el stdout para un cierto programa!!!\n");

	llamada_al_programa_redireccionando_stdin_out_ordenando("/home/utnso/mapper.sh","salida.txt",(void *) escribirBoludecesEnArchivo);

	return EXIT_SUCCESS;
}
 */
int redirec_stdin_stdout(char *pathPrograma,char *pathArchivoSalida, char* data_bloque)
{
	void escribirArchivoEntrada(FILE *archivo, char* data_bloque)
	{
		fprintf (archivo, data_bloque);

	}
	FILE *entradaARedirigir = NULL;

	char *comandoEntero=malloc(strlen(pathPrograma)+11+strlen(pathArchivoSalida));


	sprintf(comandoEntero,"%s | sort > %s",pathPrograma,pathArchivoSalida);	//Ordena las cadenas entrantes

	entradaARedirigir = popen (comandoEntero,"w");

	if (entradaARedirigir != NULL)
	{
		escribirArchivoEntrada(entradaARedirigir, data_bloque);

		pclose (entradaARedirigir);

		free(comandoEntero);
	}
	else
	{
		printf("No se pudo ejecutar el programa!");
		return -1;
	}


	return 0;
}


