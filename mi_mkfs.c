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
#include "./bibliotecas/ficheros_basico.h"

int main(int argc, char **argv){
	
	if(argc!=3){
		
		printf("\nmi_mkfs: Error debe escribir: $ ./mi_mkfs <nombre_fichero> <cantidad_bloques>\n");
		
		return -1;
	}
	
	//cantidad_bloques
	unsigned int numBloques = atoi(argv[2]);
	//cantidad_inodos
	unsigned int ninodos = numBloques/4;

	if(numBloques==0){
		printf("\nmi_mkfs: El nº de bloques debe ser mayor a 0.\n");
		
		return -1;
	}
		
	/*ETAPA 1:
	unsigned char buf[BLOCKSIZE];
	//Rellenamos de 0's el buffer.
	memset(buf,0,BLOCKSIZE);*/
	
	//Montamos el dispositivo.
	printf("\nmi_mkfs: Montando el dispositivo...\n");	
	bmount(argv[1]);//nombre_fichero
	printf("\nmi_mkfs: El dispositivo se ha montado con éxito.\n");
	printf("\nmi_mkfs: Realizando escritura...\n");
		
	if(initSB(numBloques,ninodos)<0)return -1;
	if(initMB()<0)return -1;
	if(initAI(ninodos)<0)return -1;

	//Se escribe el directorio raíz = inodo raíz.
	reservar_inodo('d',7);
	
	/*ETAPA 1:
	//Escribimos todos los bloques en el dispositivo.
	for(int i=0; i<numBloques; i++){
		bwrite(i,buf);
	}
	*/

	//Desmontamos el dispositivo.
	printf("\nmi_mkfs: Desmontando el dispositivo.\n");
	bumount();
	
	printf("\nmi_mkfs: Dispositivo desmontado con éxito.\n");

	return 0;
}
