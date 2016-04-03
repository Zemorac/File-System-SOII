/* Nombre del fichero: leer.c
 * 
 * Descripción: Programa que exporará TODO el fichero leyendo bloque por 
 * bloque. Se le pasa como argumento el nº de inodo a leer.
 * 
 * 
 * Autor: Zemorac 
 * Fecha: 19/03/16
 * 
 * */

#include "./bibliotecas/ficheros.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	
	
	if(argc!=3){
		
		printf("\nleer: Error debe escribir: $ ./leer <nombre_fichero> <nº de inodo>\n");
		
		return -1;
	}
	char string[128];
	struct INODO inodo;
	int leidos = 0;
	unsigned int ninodo = atoi(argv[2]);
	unsigned int nbytes = BLOCKSIZE;
	char salida[nbytes];
	unsigned int offset = 0;
	
	//Montamos el dispositivo.
	printf("\nleer: Montando el dispositivo...\n");	
	bmount(argv[1]);//nombre_fichero
	printf("\nleer: El dispositivo se ha montado con éxito.\n\n");
	
	//Leemos el inodo.
	inodo = leer_inodo(ninodo);	
	memset(salida,0,nbytes);//Para que filtre la basura y no la muestre por pantalla.
	//Leemos el contenido del fichero=inodo.
	while(leidos<inodo.tamEnBytesLog){
		leidos += mi_read_f(ninodo,salida,offset,nbytes);
		write(1,salida,nbytes);	
		offset +=nbytes;
	}
	sprintf(string, "\n\nNº bytes leídos: %d.\n", leidos);
	write(2, string, strlen(string));
	sprintf(string,"Tamaño en bytes lógicos del inodo %i: %d.\n",ninodo,inodo.tamEnBytesLog);
	write(2,string, strlen(string));
	
	//Desmontamos el dispositivo.
	printf("\nleer: Desmontando el dispositivo...\n\n");
	bumount();
	printf("\nleer: Dispositivo desmontado con éxito.\n");
	
	
	return 0;
}
