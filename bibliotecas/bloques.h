/* Nombre del fichero: bloques.h
 * 
 * Descripción: Cabecera con la declaración de las funciones y constantes
 * básicas del sistema de ficheros.
 * 
  * Autor: Zemorac 
 * Fecha: 10/02/16
 * 
 * */

#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /* Modos de apertura y función open()*/



#define BLOCKSIZE 1024 //Tamaño de bloque en bytes

//SISTEMA DE FICHEROS
int bmount(const char *camino);
int bumount();
int bwrite(unsigned int nbloque, const void *buf);
int bread(unsigned int nbloque, void *buf);
