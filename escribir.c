/* Nombre del fichero: escribir.c
 * 
 * Descripción: Programa que escribirá texto en uno o varios ficheros haciendo 
 * uso de reservar_inodo ('f', 6) para obtener un nº de inodo, que mostraremos
 * por pantalla y además utilizaremos como parámetro para mi_write_f.
 * 
 * 
 * Autor: Zemorac 
 * Fecha: 19/03/16
 * 
 * */
 
#include "./bibliotecas/ficheros.h"
#include <string.h>
#include <stdlib.h>
int main(int argc, char **argv){

	
	if(argc<3){
		printf("\nescribir: Error debe escribir: $ ./escribir <nombre_fichero> <offset> <inodo(opcinal)>\n");
		printf("*Si no indica el inodo el sistema se encargará de reservar un inodo nuevo.\n");
		return -1;
	}
	struct INODO inodo;
	int ninodo;
	char ruta[128];
	long nbytes;
	long offset = atoi(argv[2]);
	printf("\nescribir: Escribe la ruta del fichero: ");
	scanf("%s",ruta);
	FILE *fichero = fopen(ruta,"r");
	fseek(fichero, 0, SEEK_END); // Colocar el cursor al final del fichero
	nbytes = ftell(fichero); // Tamaño en bytes
	printf("\nescribir: El tamaño del fichero es: %ld\n",nbytes);
	char entrada[nbytes];
	fseek(fichero,0,0);
	fscanf(fichero,"%s",entrada);
	fclose(fichero);
	
	//Montamos el dispositivo.
	printf("\nescribir: Montando el dispositivo...\n");	
	bmount(argv[1]);//nombre_fichero
	printf("\nescribir: El dispositivo se ha montado con éxito.\n");
	
	//Si el inodo se pasa como argumento
	if(argc == 4){
		ninodo = atoi(argv[3]);
	}else{
		//Reservamos el inodo.
		ninodo = reservar_inodo('f',6);
	}

	inodo = leer_inodo(ninodo);
	
	//IMPRIMIR INODO 
	printf("\nNº INODO %i:\n",ninodo);
	printf("Tipo: %c\n",inodo.tipo);
	printf("Permisos: %i\n",inodo.permisos);
	printf("Nº enlaces: %i\n",inodo.nlinks);
	printf("Bytes lógicos: %i\n",inodo.tamEnBytesLog);
	printf("Bloques ocupados: %i\n",inodo.numBloquesOcupados);
	//ESCRITURA 
	if(mi_write_f(ninodo,entrada,offset,nbytes)!=nbytes){
		printf("\nescribir: ERROR, no se ha escrito en el fichero correctamente.\n");
		return -1;
	}
	//LECTURA 
	printf("\nescribir:Se ha leído lo siguiente con el offset %ld:\n",offset);
	if(mi_read_f(ninodo,entrada,offset,nbytes)!=nbytes){
		printf("\nescribir: ERROR, no se ha leído en el fichero correctamente.\n");
		return -1;
	}
	write(1,entrada,nbytes);
	printf("\n\nInodo Actualizado:\n");
	inodo = leer_inodo(ninodo);
	//IMPRIMIR INODO
	printf("\nNº INODO %i:\n",ninodo);
	printf("Tipo: %c\n",inodo.tipo);
	printf("Permisos: %i\n",inodo.permisos);
	printf("Nº enlaces: %i\n",inodo.nlinks);
	printf("Bytes lógicos: %i\n",inodo.tamEnBytesLog);
	printf("Bloques ocupados: %i\n",inodo.numBloquesOcupados);
	
	//Desmontamos el dispositivo.
	printf("\nescribir: Desmontando el dispositivo...\n\n");
	bumount();
	printf("\nescribir: Dispositivo desmontado con éxito.\n");
	
	return 0;
}
