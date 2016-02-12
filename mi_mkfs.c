/* Nombre del fichero: mi_mkfs.c
 * 
 * Descripción: Crea el dispositivo (fichero) con el tamaño adecuado.
 * 
 * Autor: Zemorac
 * Fecha: 10/02/16
 * 
 * */

#include <stdlib.h>
#include <string.h> //Para memset
#include "./bibliotecas/bloques.h"

int main(int argc, char **argv){
	
	if(argc!=3){
		printf("\n Error debe escribir: $ ./mi_mkfs <nombre_fichero> <cantidad_bloques>\n");
		
		return -1;
	}
	
	//cantidad_bloques
	unsigned int numBloques = atoi(argv[2]);
	
	
	if(numBloques==0){
		printf("El nº de bloques debe ser mayor a 0.\n");
		
		return -1;
	}
	
	printf("OK\n");	
	unsigned char buf[BLOCKSIZE];
	//Rellenamos de 0's el buffer.
	memset(buf,0,BLOCKSIZE);
	
	//Montamos el dispositivo.
	printf("Montando el dispositivo...\n");	
	bmount(argv[1]);//nombre_fichero
	printf("Realizando escritura...\n");
		
	//Escribimos todos los bloques en el dispositivo.
	for(int i=0; i<numBloques; i++){
		bwrite(i,buf);
	}
	
	//Cerramos el dispositivo.
	bumount();
	
	printf("Dispositivo cerrado con éxito.\n");
}
