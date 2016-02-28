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
	sb.posPrimerInodoLibre = 0;
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
 * Parámetros: (Ninguno)
 * 			
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
	
	for(int j=posSB; j<= sb.posUltimoBloqueAI; j++){
		
		escribir_bit(j,1);
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

/* Función escribir_bit: Escribe en un bit del Mapa de Bits, que representa 
 * a un bloque, el valor (0 or 1) que indica si un bloque está libre u ocupado.
 * 
 * Parámetros:
 * 			-nbloque : # bloque específico del sistema de ficheros.
 *			-bit 	 : valor (0 or 1) que representa al bloque en el Mapa de Bits. 
 * */
int escribir_bit(unsigned int nbloque, unsigned int bit){

	struct SUPERBLOQUE sb;
	unsigned int posByte,posBit,posBloqueMB, byteMB;
	unsigned int bloqueMB[BLOCKSIZE];
	unsigned char mascara = 128; //100000000

	if(bread(posSB,&sb)<0){

		printf("\nescribir_bit(): ERROR, no se ha leído el SUPERBLOQUE.\n");

		return -1;
	}
	// Calculamos en qué byte nos encontramos, dentro del Mapa de Bits, según el número de bloque.
	posByte = nbloque/8;
	// Calculamos la posición del bit deseado en el byte del Mapa de Bits.
	posBit = nbloque%8;

	//Determinamos que BLOQUE DEL MAPA DE BITS hemos de leer.
	//¿En qué bloque del MAPA DE BITS cae posByte?
	posBloqueMB = (posByte/BLOCKSIZE) + sb.posPrimerBloqueMB;
	
	//Leemos el bloque del MAPA DE BITS
	if(bread(posBloqueMB, bloqueMB) < 0 ){
		
		printf("\n escribir_bit(): ERROR, no se ha leído el bloque del MAPA DE BITS. \n");
			
			return -1;
		}
		
		byteMB = posByte % BLOCKSIZE; // byte del MB en el cual está el bit a escribir
										
		if (bit == 1){
		
			mascara >>= posBit; // desplazamiento de bits a la derecha
			bloqueMB[byteMB] |= mascara;// operador OR para bits
		
		}
		
		else if(bit == 0){

			mascara >>= posBit; // desplazamiento de bits a la derecha
			bloqueMB[byteMB] &= ~mascara; //operadores AND y NOT para bits
			
		}
			
		
		if(bwrite(posBloqueMB, bloqueMB) < 0){

			printf("\nescribir_bit(): ERROR, no se ha escrito el mapa de bits.\n");
			return -1;
	}
	
							
		
	return 0; 
}

/* Función leer_bit: Lee y retorna un bit del Mapa de Bits, que representa 
 * a un bloque, el valor (0 or 1) que indica si un bloque está libre u ocupado.
 * Parámetros:
 * 			-nbloque : # bloque específico del sistema de ficheros.
 * */
unsigned char leer_bit(unsigned int nbloque){

	struct SUPERBLOQUE sb;
	unsigned int posByte,posBit,posBloqueMB,byteMB;
	unsigned int bloqueMB[BLOCKSIZE];
	unsigned char mascara = 128; //100000000

	if(bread(posSB,&sb)<0){

		printf("\nescribir_bit(): ERROR, no se ha leído el SUPERBLOQUE.\n");

		return -1;
	}
	
	// Calculamos la posición del byte, en el bloque, donde está nuestro bit.
	posByte = nbloque/8;
	posBit = nbloque%8;

	//Determinamos que BLOQUE DEL MAPA DE BITS hemos de leer.
	//¿En qué bloque del MAPA DE BITS cae posByte?
	posBloqueMB = (posByte/BLOCKSIZE) + sb.posPrimerBloqueMB;
	
	//Leemos el bloque del MAPA DE BITS
	if(bread(posBloqueMB, bloqueMB) < 0 ){
		
		printf("\n escribir_bit(): ERROR, no se ha leído el bloque del MAPA DE BITS. \n");
			
			return -1;
		}
		byteMB = posByte % BLOCKSIZE; // byte del MB en el cual esta el bit a escribir
		
		mascara >>= posBit; // desplazamiento de bits a la derecha
		mascara &= bloqueMB[byteMB]; // operador AND para bits	
		mascara >>= (7-posBit); // desplazamiento de bits a la derecha		
													
		
	return mascara;
	
}

/* Función reservar_bloque: Se encarga de buscar el primer 0 en el Mapa de Bits,
 * es decir, el primer bloque libre. Reserva el bloque libre escribiendo un 1 en
 * la posición del Mapa de bits y devuelve la posición del bloque (nº bloque que
 * hemos reservado = posíción lógica).
 * 
 * Parámetros: (Ninguno)
 * 			
 * */
int reservar_bloque(){

	struct SUPERBLOQUE sb;
	unsigned int bloqueMB[BLOCKSIZE];
	unsigned int bufferAux[BLOCKSIZE];
	unsigned int posBit,numBloque;
	unsigned char mascara = 128; //100000000

	if(bread(posSB,&sb)<0){

		printf("\nreservar_bloque(): ERROR, no se ha leído el SUPERBLOQUE.\n");

		return -1;
	}
	
	if(sb.cantBloquesLibres == 0){
		
		printf("reservar_bloque: ERROR, no quedan bloques libres para reservar.\n");
		
		return -1;
	}
	//Rellenamos de 1's el buffer auxiliar.
	memset(bufferAux,255,BLOCKSIZE);
	//Leemos los bloques del MAPA DE BITS.
	for(int i=sb.posPrimerBloqueMB; i <= sb.posUltimoBloqueMB; i++){
		
		
		if(bread(i,bloqueMB) < 0){
			
			printf("reservar_bloque: ERROR, no se ha podido leer el bloque %i del MAPA DE BITS.\n", i);
			
			return -1;
		}
		//Si el bloque leído contiene algún 0...
		if(memcmp(bufferAux,bloqueMB,BLOCKSIZE) == 0){

			printf("reservar_bloque: Bloque para reservar encontrado.\nReservando bloque...\n");
			//Recorremos los bytes del bloque leído del MAPA DE BITS.
			for (int j = 0; j < BLOCKSIZE; j++){
					//bloqueMB[j] = un byte del bloque del MAPA DE BITS.
				if (bloqueMB[j] < 255) { // hay bits a 0 en el byte
					posBit = 0;
					while (bloqueMB[j] & mascara) { // operador AND para bits
						//Vamos "empujando" el byte hacia la izquierda hasta que quede el 0 primero.
						bloqueMB[j] <<= 1;
						posBit++;
					}
					//Cálculo del bloque que podemos reservar en el MAPA DE BITS:
					// i = bloqueMB
					// j = posByte
					numBloque = ((i - sb.posPrimerBloqueMB) * BLOCKSIZE + j) * 8 + posBit;
					//Bloque ahora ocupado
					if(escribir_bit(numBloque,1)<0){

						printf("reservar_bloque(): ERROR, no se ha podido reservar el bloque.\n");

						return -1;
					}
					//Ahora hay un bloque libre menos
					sb.cantBloquesLibres--;
					//Actualizamos el SUPERBLOQUE
					if(bread(posSB,&sb)<0){

						printf("\nreservar_bloque(): ERROR, no se ha actualizado el SUPERBLOQUE.\n");

						return -1;
					}
					//Devolvemos el número de bloque reservado.
					return numBloque; 
				}
			}
			
		}

			
		
	}

	printf("reservar_bloque: ERROR, no se ha podido realizar la reserva de un bloque.\n");

	return -1;
}

/* Función liberar_bloque: Se encarga de buscar el bit que representa al número
 * de bloque lógico pasado por parámetro, en el Mapa de Bits, para escribir un 
 * 0 y dejarlo como bloque libre. Retorna ĺa posición de bloque liberado (nº 
 * bloque que hemos liberado = posición lógica).
 * 
 * Parámetros:
 * 			-nbloque : # bloques del sistema de ficheros.
 * */
int liberar_bloque(unsigned int nbloque){

	struct SUPERBLOQUE sb;

	if(bread(posSB,&sb)<0){

		printf("\nliberar_bloque(): ERROR, no se ha leído el SUPERBLOQUE.\n");

		return -1;
	}

	if(escribir_bit(nbloque,0)<0){

		printf("liberar_bloque(): ERROR, no se ha podido liberar el bloque.\n");

		return -1;
	}

	//Ahora hay un bloque más libre
	sb.cantBloquesLibres++;
	//Actualizamos el SUPERBLOQUE
	if(bread(posSB,&sb)<0){

		printf("\nliberar_bloque(): ERROR, no se ha actualizado el SUPERBLOQUE.\n");

		return -1;
	}

	return nbloque;
}

/* Función escribir_inodo: Escribe el contenido de una variable de tipo 
 * INODO en un determinado inodo del array de inodos.
 * 
 * Parámetros:
 * 			-inodo : variable tipo inodo.
 *			-ninodo: # inodo en el Array de Inodos.
 * */
int escribir_inodo(struct INODO inodo, unsigned int ninodo){

	struct SUPERBLOQUE sb;
	struct INODO bloqueInodo[BLOCKSIZE/T_INODO];
	unsigned int numBloque;

	if(bread(posSB,&sb)<0){

		printf("\nescribir_inodo(): ERROR, no se ha leído el SUPERBLOQUE.\n");

		return -1;
	}
	//Obtenemos el número de bloque en el se encuentra el inodo parametrizado.
	numBloque = ((ninodo*T_INODO)/BLOCKSIZE) + sb.posPrimerBloqueAI;
	//Leemos el bloque específico.
	if(bread(numBloque,bloqueInodo)<0){

		printf("\nescribir_inodo(): ERROR, no se ha leído el bloque %i.\n",numBloque);

		return -1;
	}
	//Escribimos el inodo en bloqueInodo
	bloqueInodo[ninodo%(BLOCKSIZE/T_INODO)] = inodo;
	//Escribimos en el dispositivo el inodo en el Array de Inodos
	if(bwrite(numBloque,bloqueInodo)<0){

		printf("\nescribir_inodo(): ERROR, no se ha escrito el bloque %i actualizado.\n",numBloque);

		return -1;
	}

	printf("\nescribir_inodo(): Se ha escrito el inodo %i con éxito.\n",ninodo);

	return 0;
}

/* Función leer_inodo: Lee un determinado inodo del array de inodos para 
 * volcarlo en una variable de tipo INODO. Retorna la variable de tipo 
 * inodo.
 * 
 * Parámetros:
 * 			-ninodo: # inodo en el Array de Inodos.
 * */
struct INODO leer_inodo(unsigned int ninodo){
	
	struct SUPERBLOQUE sb;
	struct INODO bloqueInodo[BLOCKSIZE/T_INODO];
	unsigned int numBloque;

	if(bread(posSB,&sb)<0){

		printf("\nleer_inodo(): ERROR, no se ha leído el SUPERBLOQUE.\n");
	}

	//Obtenemos el número de bloque en el se encuentra el inodo parametrizado.
	numBloque = ((ninodo*T_INODO)/BLOCKSIZE) + sb.posPrimerBloqueAI;
	//Leemos el bloque específico.
	if(bread(numBloque,bloqueInodo)<0){

		printf("\nescribir_inodo(): ERROR, no se ha leído el bloque %i.\n",numBloque);

	}

	printf("\nleer_inodo(): Se ha leído el inodo %i con éxito.\n",ninodo);

	return bloqueInodo[ninodo%(BLOCKSIZE/T_INODO)];
}

/* Función reservar_inodo: Encuentra el primer inodo libre (información 
 * almacenada en el superbloque), lo reserva (con la ayuda de la 
 * función escribir_inodo) y devuelve su número.
 * 
 * Parámetros:
 * 			-tipo 	 : metainformación del inodo.
 *			-permisos: metainformación del inodo.
 * */
int reservar_inodo(unsigned char tipo, unsigned char permisos){

	struct SUPERBLOQUE sb;
	struct INODO bloqueInodo[BLOCKSIZE/T_INODO];
	struct INODO inodoAreservar;
	unsigned int posInodoReservado;

	if(bread(posSB,&sb)<0){

		printf("\nreservar_inodo(): ERROR, no se ha leído el SUPERBLOQUE.\n");

		return -1;
	}

	posInodoReservado = sb.posPrimerInodoLibre;
	//Reservamos el primer inodo libre indicado por la lista enlazada de inodos libres que está en el SUPERBLOQUE.
	inodoAreservar = leer_inodo(sb.posPrimerInodoLibre);
	//Ahora el primer inodo libre será el que apunte el inodo a reservar.
	sb.posPrimerInodoLibre = inodoAreservar.punterosDirectos[0];
	//Reducimos la cantidad de inodos libres.
	sb.cantInodosLibres--;
	//Modificamos el inodo a reservar
	inodoAreservar.tipo = tipo;
    inodoAreservar.permisos = permisos;
    inodoAreservar.nlinks = 1;
    inodoAreservar.tamEnBytesLog = 0;
    inodoAreservar.atime = time(NULL); 
	inodoAreservar.mtime = time(NULL); 
	inodoAreservar.ctime = time(NULL);
    inodoAreservar.numBloquesOcupados = 0;
    /*Inicializamos los punteros, habiendo salvado antes el número del siguiente inodo libre
    que ahora apunta sb.primerInodoLirbe el SUPERBLOQUE. Por tanto ya podemos usar la posición
    punterosDirestos[0] del inodo a reservar.
    */
    for(int i = 0; i < T_DIRECTOS; i++) inodoAreservar.punterosDirectos[i] = 0;
	for(int i = 0; i < T_INDIRECTOS; i++) inodoAreservar.punterosIndirectos[i] = 0;

    //Y reservamos el inodo.
	if(escribir_inodo(inodoAreservar,sb.posPrimerInodoLibre)<0){

		printf("\nreservar_inodo(): ERROR, no se ha escrito el inodo con éxito.\n");

		return -1;
	}
	
	//Actualizamos el SUPERBLOQUE
	if(bread(posSB,&sb)<0){

		printf("\nreservar_inodo(): ERROR, no se ha actualizado el SUPERBLOQUE.\n");

		return -1;
	}

	printf("\nreservar_inodo(). El inodo %i se ha reservado con éxito.\n",posInodoReservado);

 	return posInodoReservado;
}



