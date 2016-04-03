/* Nombre del fichero: fichero.c
 * 
 * Descripción: Declaración de las funciones de la zona de datos del 
 * sistema de ficheros.
 * 
 * 
 * Autor: Zemorac 
 * Fecha: 16/03/16
 * 
 * */

#include "bloques.h"
#include "ficheros_basico.h"

//ETAPA 5 
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
