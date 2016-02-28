/* Nombre del fichero: bloques.c
 * 
 * Descripción: Descripción de las funciones básicas de E/S de bloques
 * del sistema de ficheros.
 * 
 * Autor: Celia Caro 
 *		  Sebastián Elías Oporto 
 * Fecha: 10/02/16
 * 
 * */


#include "bloques.h"

static int descriptor = 0;

// Funciones sobre el sistema de ficheros

/* Función bmount: Monta/abre el disco.imagen (fichero) que se usará
 * como simulador del disco duro (HD).
 * 
 * Parámetros:
 * 			-Camino: Puntero a la ruta donde se creará el disco.imagen
 * */
int bmount(const char *camino){
	/* Open hace una llamada al sistema. Establece una conexión 
	 * entre el fichero y su descriptor de fichero. El sistema tiene una
	 * tabla con los descriptores de ficheros que se van abriendo (0-19),
	 * 0: entrada estándar, 1: salida estándar, 2: salida de error estándar.
	 * 
	 * */
	if((descriptor = open(camino,O_RDWR|O_CREAT, 0666))<0){
		printf("bmount: Se ha producido un error al abrir el dispositivo.\n");
	}

	return descriptor;
}

/* Función bmount: Cierra el disco.imagen (fichero) que se usará como 
 * simulador del disco duro (HD).
 * 
 * Parámetros: (Ninguno)
 * 							
 * */
int bumount(){
	
	if(close(descriptor)<0){
		printf("bumount: Se ha producido un error al cerrar el dispositivo.\n");
		return -1;
	}
	
	return 0;
}
 
 /* Función bwrite: Escribe el contenido de un buffer de memoria del
  * tamaño de un bloque, en un bloque determinado del sistema de ficheros.
  * 
  * Parámetros: 
  * 		-nbloque: # bloque específico, del sistema de ficheros, a escribir.
  * 		-buf	: Puntero al buffer de memoria, del tamaño de BLOCKSIZE,
  * 				  que contiene los datos a escribir en el sistema de ficheros.
  * Retorna el # de bytes escritos.
  * 
  * */
 int bwrite(unsigned int nbloque, const void *buf){
	 /* Calculamos el desplazamiento que se tiene que hacer para posicionar
	  * el puntero de escritura en la posición correcta del SF para escribir el
	  * contenido del buffer de memoria, que contiene los datos.
	  * 
	  * */
	 
	 int desplazamiento = nbloque*BLOCKSIZE;
	 
	 //El punto de referencia es el inicio del fichero + llamada al sistema.
	 lseek(descriptor,desplazamiento, SEEK_SET);
	 //Escribimos = llamada al sistema
	 if(write(descriptor,buf,BLOCKSIZE)<0){
		printf("bwrite: Se ha producido un error en la escritura de un bloque en el dispositivo.\n");
		
		return -1;
	 }
	 
	 //Si se han escrito correctamente los bytes.
	 return BLOCKSIZE;
	 
 }
 
 /* Función bread: Lee del sistema de ficheros el bloque especificado.
  * Copia su contenido en un buffer de memoria del tamaño de un bloque.
  * 
  * Parámetros:
  * 		- nbloque:  # bloque específico, del sistema de ficheros, a leer.
  * 		- buf	 : Puntero al buffer de memoria, del tamaño de BLOCKSIZE,
  * 				   que contiene los datos a leer en el sistema de ficheros.
  * Retorna el # de bytes leídos.
  * 
  * */
int bread(unsigned int nbloque, void *buf){
	int desplazamiento = nbloque*BLOCKSIZE;
	//El punto de referencia es el inicio del fichero + llamada al sistema.
	lseek(descriptor,desplazamiento, SEEK_SET);
	//Leemos = llamada al sistema. 
	if(read(descriptor,buf, BLOCKSIZE)<0){
		printf("bread: Se ha producido un error en la lectura de un bloque en el dispositivo.\n");
		
		return -1;
	}
	
	//Si se han leído correctamente los bytes.
	return BLOCKSIZE;
	
}






