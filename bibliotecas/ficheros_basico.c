/* Nombre del fichero: ficheros_basico.c
 * 
 * Descripción: Definición de las funciones de la zona de metadatos
 * del sistema de ficheros.
 * 
 * Autor: Zemorac 
 * Fecha: 17/02/16
 * 
 * */

#include "ficheros_basico.h"
#include "bloques.h"
#include <string.h>
#include <limits.h>

//Funciones sobre el sistema de ficheros

/* Función tamMB: Calcula el tamaño en bloques necesarios para
 * dar tamaño al Mapa de Bits.
 * 
 * Parámetros:
 * 			-nbloques: # bloques del sistema de ficheros.
 * */
int tamMB (unsigned int nbloques){

	unsigned int tam=0;

		if((nbloques/8) % BLOCKSIZE == 0){
			
			tam = (nbloques/8)/BLOCKSIZE;
		}
		
		else{
			
			tam = ((nbloques/8)/BLOCKSIZE) + 1; 
		}

	return tam;
}

/* Función tamAI: Calcula el tamaño en bloques necesarios para
 * dar tamaño al Array de Inodos.(Heurística)
 * 
 * Parámetros:
 * 			-ninodos: # inodos del sistema de ficheros.
 * */
int tamAI (unsigned int ninodos){

	int tam=0;
	
		if((ninodos * T_INODO) % BLOCKSIZE == 0){
			
			tam = (ninodos * T_INODO) / BLOCKSIZE;
		} 
		else{
			
			tam = (ninodos * T_INODO) / BLOCKSIZE + 1;
		}
	
	
	return tam;


}

/* Función initSB: Se encarga de inicializar el SUPERBLOQUE.
 * 
 * Parámetros:
 * 			-nbloques: # bloques del sistema de ficheros.
 *			-ninodos : # inodos del sistema de ficheros.
 * */
int initSB(unsigned int nbloques, unsigned int ninodos){
	

	struct SUPERBLOQUE sb;

	sb.posPrimerBloqueMB = posSB + tamSB;
	sb.posUltimoBloqueMB = sb.posPrimerBloqueMB + tamMB(nbloques) -1;
	sb.posPrimerBloqueAI = sb.posUltimoBloqueMB +1;
	sb.posUltimoBloqueAI = sb.posPrimerBloqueAI + tamAI(ninodos) -1;
	sb.posPrimerBloqueDatos = sb.posUltimoBloqueAI +1;	
	sb.posUltimoBloqueDatos = nbloques-1;
	sb.posInodoRaiz = 0;
	sb.posPrimerInodoLibre = 1;
	sb.cantBloquesLibres = nbloques-1;//Menos bloque del SUPERBLOQUE
	sb.cantInodosLibres = ninodos-1;// Menos INODO RAÍZ
	sb.totBloques = nbloques;
	sb.totInodos = ninodos;

	//Escribimos el SUPERBLOQUE en el sistema de ficheros
	if(bwrite(posSB, &sb) == -1){
		
		printf("\ninitSB(): ERROR, no se ha podido escribir el SUPERBLOQUE.\n");
		
		return -1;
	}		
			
			
	printf("\ninitSB(): Se ha escrito el SUPERBLOQUE en el sistema de ficheros.\n\n");
		

	return 0;
}

/* Función initMB: Se encarga de inicializar el Mapa de Bits.
 * 
 * Parámetros:
 * 			-nbloques: # bloques del sistema de ficheros.
 * */
int initMB(){

	unsigned char mb[BLOCKSIZE];
	struct SUPERBLOQUE sb;
	
	if(bread(posSB, &sb) == -1){
		
		printf("\ninitMB(): ERROR, no se ha leido el SUPERBLOQUE.\n");
		
		return -1;
	}
		
	memset(mb,0,BLOCKSIZE);
	
	for(int i=sb.posPrimerBloqueMB; i <= sb.posUltimoBloqueMB; i++){
	
		if(bwrite(i, mb) == -1){
			
			printf("\ninitMB(): ERROR, no se ha escrito el Mapa de Bits. \n");
			
			return -1;
		}

		sb.cantBloquesLibres--;
		printf("initMB(): Se ha escrito el bloque %i en el MAPA DE BITS.\n",i);
	}
	
	//Actualizamos el SUPERBLOQUE del sistema de ficheros
	if(bwrite(posSB, &sb) == -1){
		
		printf("\ninitMB(): ERROR, no se ha podido escribir el SUPERBLOQUE. \n");
		
		return -1;
	}		
					
	printf("\ninitMB(): Se ha actualizado el SUPERBLOQUE en el sistema de ficheros. \n");

	return 0;
}

/* Función initAI: Se encarga de inicializar el Array de Inodos.
 * 
 * Parámetros:
 * 			-ninodos : # inodos del sistema de ficheros.
 * */
int initAI(unsigned int ninodos){

	struct SUPERBLOQUE sb;
	struct INODO bloqueInodos[BLOCKSIZE/T_INODO]; //Cada (BLOCKSIZE/T_INODO) inodos es un bloque.
	unsigned int sgLibre;
	unsigned int inodosPorBloque = BLOCKSIZE/T_INODO;

	//Leemos el SUPERBLOQUE para localizar el Array de Inodos
	if(bread(posSB, &sb) == -1){

		printf("\ninitAI(): ERROR, no se ha leido el SUPERBLOQUE.\n");
		
		return -1;
	}

	sgLibre = sb.posPrimerInodoLibre;

	for(int i=sb.posPrimerBloqueAI; i<=sb.posUltimoBloqueAI; i++){

		for (int j = 0; j < inodosPorBloque; ++j){
			
			bloqueInodos[j].tipo = 'l'; //Tipo inodo: Libre.

			if (sgLibre < ninodos){ //Para saber por el inodo que vamos sobre el total.

				bloqueInodos[j].punterosDirectos[0] = sgLibre; //Asignamos la posición del siguiente inodo libre.
				sgLibre++;
			}
			else{

				bloqueInodos[j].punterosDirectos[0] = UINT_MAX; //Asignamos máximo valor para un int.

			}
			
			
		}

		// Escribimos cada bloque de inodos.
		if(bwrite(i,bloqueInodos)<0){

			printf("\ninitAI(): ERROR, no se ha escrito el bloque de inodos correctamente.\n");

			return -1;
		}
		sb.cantBloquesLibres--;
		//Comprueba que se escriben los bloques correctamente:
		//printf("initAI(): Se ha escrito el bloque %i en el ARRAY DE INODOS.\n",i);
	}

	//Actualizamos el SUPERBLOQUE en el sistema de ficheros
	if(bwrite(posSB, &sb) == -1){
		
		printf("\ninitAI(): ERROR, no se ha podido escribir el SUPERBLOQUE.\n");
		
		return -1;
	}		
			
			
	printf("\ninitAI(): Se ha actualizado el SUPERBLOQUE en el sistema de ficheros.\n");


	return 0;

}





