/* Nombre del fichero: ficheros_basico.h
 * 
 * Descripción: Definición de las diferentes estructuras del sistema de
 * ficheros a utilizar. Declaración de funciones de la zona de metadatos
 * del sistema de ficheros.
 *
 * Autor: Zemorac 
 * Fecha: 17/02/16
 * 
 * */

#include <time.h>
#include "bloques.h"


#define posSB 0 //el superbloque se escribe en el primer bloque de nuestro FS
#define T_INODO 128 //tamaño en bytes de un inodo
#define tamSB 1
#define T_DIRECTOS 12
#define T_INDIRECTOS 3

struct SUPERBLOQUE{
	unsigned int posPrimerBloqueMB; //Posición del primer bloque del mapa de bits
	unsigned int posUltimoBloqueMB; //Posición del último bloque del mapa de bits
	unsigned int posPrimerBloqueAI; //Posición del primer bloque del array de inodos
	unsigned int posUltimoBloqueAI; //Posición del último bloque del array de inodos
	unsigned int posPrimerBloqueDatos; //Posición del primer bloque de datos
	unsigned int posUltimoBloqueDatos; //Posición del último bloque de datos
	unsigned int posInodoRaiz; //Posición del inodo del directorio raíz
	unsigned int posPrimerInodoLibre; //Posición del primer inodo libre
	unsigned int cantBloquesLibres; //Cantidad de bloques libres
	unsigned int cantInodosLibres; //Cantidad de inodos libres
	unsigned int totBloques; //Cantidad total de bloques
	unsigned int totInodos; //Cantidad total de inodos
	char padding[BLOCKSIZE-12*sizeof(unsigned int)]; //Relleno
};

struct INODO{ 
	unsigned char tipo; //Tipo (libre, directorio o fichero)
	unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecución)
	unsigned char reservado_alineacion1 [6];

	time_t atime; //Fecha y hora del último acceso a datos: atime
	time_t mtime; //Fecha y hora de la última modificación de datos: mtime
	time_t ctime; //Fecha y hora de la última modificación del inodo: ctime

	unsigned int nlinks; //Cantidad de enlaces de entradas en directorio
	unsigned int tamEnBytesLog; //Tamaño en bytes lógicos
	unsigned int numBloquesOcupados; //Cantidad de bloques ocupados en la zona de datos

	unsigned int punterosDirectos[12]; //12 punteros a bloques directos
	unsigned int punterosIndirectos[3]; //3 punteros a bloques indirectos:1 puntero indirecto simple, 1 puntero indirecto doble, 1 puntero indirecto triple 

	char padding[T_INODO-2*sizeof(unsigned char)-3*sizeof(time_t)-18*sizeof(unsigned int)-6*sizeof(unsigned char)];

};

struct STAT{
	unsigned char tipo; //Tipo (libre, directorio o fichero)
	unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecución)
	unsigned char reservado_alineacion1 [6];

	time_t atime; //Fecha y hora del último acceso a datos: atime
	time_t mtime; //Fecha y hora de la última modificación de datos: mtime
	time_t ctime; //Fecha y hora de la última modificación del inodo: ctime

	unsigned int nlinks; //Cantidad de enlaces de entradas en directorio
	unsigned int tamEnBytesLog; //Tamaño en bytes lógicos
	unsigned int numBloquesOcupados; //Cantidad de bloques ocupados en la zona de datos
};

//FUNCIONES

//ETAPA 2
int tamMB (unsigned int nbloques);
int tamAI (unsigned int ninodos);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB();
int initAI(unsigned int ninodos);
//ETAPA 3
int escribir_bit(unsigned int nbloque, unsigned int bit);
unsigned char leer_bit(unsigned int nbloque);
int reservar_bloque();
int liberar_bloque(unsigned int nbloque);
int escribir_inodo(struct INODO inodo, unsigned int ninodo);
struct INODO leer_inodo(unsigned int ninodo);
int reservar_inodo(unsigned char tipo, unsigned char permisos);
//ETAPA 4
int obtener_rangoBL(struct INODO inodo, unsigned int nblogico,int *ptr);
int obtener_indice(unsigned int nblogico, int nivel_punteros);
int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, char reservar);
int liberar_bloques_inodo(unsigned int ninodo, unsigned int blogico,int ptr,int rangoBL,int nivel);
int liberar_inodo(unsigned int ninodo);
int eliminar_bloque_indice(int nivel,unsigned int ninodo,struct SUPERBLOQUE *sb,struct INODO *inodo, int ptr, int *rango, int ptr_ant,unsigned int blogico);
int preservar_bloque(unsigned int blogico,unsigned int *bloque_indice, int ptr, unsigned int ninodo,int rango,int nivel);
