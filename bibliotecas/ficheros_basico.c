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
 * hemos reservado = posición lógica).
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
	//struct INODO bloqueInodo[BLOCKSIZE/T_INODO];
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

/* Función obtener_rangoBL: Asocia un nivel a acada rango de bloques
 * lógicos. Si podemos direccionar solo el primer conjunto de bloques 
 * índices entonces rango = 1, si podemos hasta el segundo conjunto de 
 * bloques índices entonces rango = 2 y si podemos hasta el tercer 
 * conjunto de bloques índices entonces rango = 3. Para bloques directos 
 * rango = 0.
 * 
 * Parámetros:
 * 			-inodo 	 : inodo específico.
 *			-nblogico: # bloque lógico.
 * 			-*ptr	 : puntero a índices del inodo.
 * 
 * */
int obtener_rangoBL(struct INODO inodo, int nblogico, int *ptr){
	
	unsigned int npunteros,rangoBL,directos,indirectos0,indirectos1,indirectos2;
	
	/* # punteros que caben en un bloque de tamaño BLOCKSIZE,
	 * los bloques que contienen punteros son bloques índices.
	 * En este caso 256.
	 * */
	npunteros = BLOCKSIZE/sizeof(int);
	
	//Cálculo de bloques que se pueden direccionar.
	directos = T_DIRECTOS;
	//Necesitan un solo bloque de índices. (268)
	indirectos0 = directos + npunteros;
	//Necesitan un bloque índice por cada índice del bloque de índices que es apuntado. (65.804)
	indirectos1 = indirectos0 + (npunteros*npunteros); 
	//Idem que el caso anterior pero con más bloques índices. (16.843.020)
	indirectos2 =  indirectos1 + (npunteros*npunteros*npunteros);
	
	//Si nuestro bloque lógico pertenece a bloques lógicos directos...
	if(nblogico<directos){
		
		//Proporcionamos el índice al bloque de datos.
		*ptr = inodo.punterosDirectos[nblogico];
		//Directos tendrán el rango 0.
		rangoBL = 0; 
	}
	//Si nuestro bloque lógico pertenece a bloques lógicos indirectos...
	else {
		//de nivel 0:
		if(nblogico<indirectos0){
			//Proporcionamos el índice al primer bloque de índices.
			*ptr = inodo.punterosIndirectos[0];
			//Indirectos0 tendrán el rango 1.
			rangoBL = 1;
		}
		else {
			//de nivel 1:
			if(nblogico<indirectos1){
				//Proporcionamos el índice al primer bloque de índices.
				*ptr = inodo.punterosIndirectos[1];
				//Indirectos1 tendrán el rango 2.
				rangoBL = 2;
			}
			else {
				//de nivel 2:
				if(nblogico<indirectos2){
					//Proporcionamos el índice al primer bloque de índices.
					*ptr = inodo.punterosIndirectos[2];
					//Indirectos2 tendrán el rango 3.
					rangoBL = 3;
				}
				else {
					*ptr = 0; //NULL
					rangoBL = -1;
					printf("\nobtener_rangoBL(): ERROR, no existe el bloque lógico %i.\n",nblogico);
				}
			}
		}
	}
	
	return rangoBL;
}

/* Función obtener_indice: Se encarga de calcular los índices, que almacenan
 * los bloques índice, que conectan con el bloque de datos.
 * 
 * Parámetros:
 *			-nblogico: # bloque lógico.
 * 			-nivel_punteros	 : nivel de punteros en el que tenemos que 
 * 							   empezar el recorrido para traducir el 
 * 							   lógico a físico.
 * 
 * Nota: Para ir obteniendo los índices se restan los bloques anteriores 
 * para poder relativizar mejor, es decir, para que caiga siempre en el 
 * rango de un bloque de punteros (0-255). Cada rango indirecto tiene
 * niveles de bloques índice que se van recorriendo desde el nivel 3 hasta
 * el nivel 1, 3->2->1(Datos). El número de niveles de bloques índice
 * depende del rango actual.
 * */
int obtener_indice(int nblogico, int nivel_punteros){
	
	unsigned int npunteros, directos, indirectos0, indirectos1, indirectos2;
	int indice;
	
	/* # punteros que caben en un bloque de tamaño BLOCKSIZE,
	 * los bloques que contienen punteros son bloques índices.
	 * En este caso 256.
	 * */
	npunteros = BLOCKSIZE/sizeof(int);
	//Cálculo de bloques que se pueden direccionar.
	directos = T_DIRECTOS;
	//Necesitan un solo bloque de índices. (268)
	indirectos0 = directos + npunteros;
	//Necesitan un bloque índice por cada índice del bloque de índices que es apuntado. (65.804)
	indirectos1 = indirectos0 + (npunteros*npunteros); 
	//Idem que el caso anterior pero con más bloques índices. (16.843.020)
	indirectos2 =  indirectos1 + (npunteros*npunteros*npunteros);
	
	// Si el índice está en el bloque de directos.
	if(nblogico < directos){ 
		
		indice = nblogico;
	}
	
	else{ // Si no está entre los bloques de directos...
		
		if(nblogico < indirectos0){ // Si está en el indirectos 0.
			//Solo tiene un nivel, nivel 1, de bloques índice.
			indice = nblogico-directos;
		}
		
		else{
		
		if(nblogico < indirectos1){ // Si está en el indirectos1
			//Solo tiene dos niveles, 2->1, de bloques índice.
			if(nivel_punteros == 2){ // Si está en el nivel 2.
				
				indice = (nblogico-indirectos0)/npunteros;
			}
			else{
				
				if(nivel_punteros == 1){ // Si está en el nivel 1.
					
					indice = (nblogico-indirectos0)%npunteros;
					
				}
			}
		}
		else{
			
			if(nblogico < indirectos2){ // Si está en el indirectos2.
				//Solo tiene 3 niveles, 3->2->1, de bloques índice.
				/*(npunteros*npunteros) = 256², no es 256, ya que
				 * necesitamos otro bloque de índice para direccionar
				 * 3 niveles de bloques índice. Para el nivel 2 solo
				 * nos bastaba con el cociente y el resto de 256.
				 * */
				if(nivel_punteros == 3){ // Si está en el nivel 3.

					indice = (nblogico-indirectos1)/(npunteros*npunteros);
					
				}
				
				else{
					
					if(nivel_punteros == 2){ // Si está en el nivel 2.
						/*Como ahora estamos en el nivel 2, ya podemos
						 * dividir entre 256(npunteros), como en el caso
						 * de indirectos1.
						 * */
						indice = ((nblogico-indirectos1)%(npunteros*npunteros))/npunteros;
						
					}
					else{
						//Solo hacemos el resto, como en el caso de indirectos1.
						if(nivel_punteros == 1){ // Si está en el nivel 1.
							
							indice = ((nblogico-indirectos1)%(npunteros*npunteros))%npunteros;
							
						}
					}
				}
			}
		}
	}
		
	}
	
	return indice;
}

/* Función traducir_bloque_inodo: Se encarga de obtener un bloque físico
 * a partir de un bloque lógico del inodo especificado.
 * 
 * Parámetros:
 * 			-ninodo		: # inodo al que pertenece el bloque lógico.
 *			-blogico	: bloque lógico a traducir.
 * 			-reservar	: indica el fin de la función, 0 consultar y 1
 * 						  consultar y en caso de que no exista el bloque
 * 						  físico reservarlo.
 * 
 * */
int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, char reservar){
	
	//Se lee el inodo
	struct INODO inodo = leer_inodo(ninodo);
	int ptr,ptr_ant,salvar_inodo,rango,nivel_punteros = 0;
	unsigned int bloque_indice[BLOCKSIZE/sizeof(int)];
	
	/*Obtenemos el número de rango en el que nos encontramos según el bloque lógico y 
	 *se asigna a ptr el valor del puntero correspondiente del inodo.
	 * */
	rango = obtener_rangoBL(inodo,blogico,&ptr);
	//Asignamos a nivel_punteros el número de rango del  bloque logico
	nivel_punteros = rango; // Valor inicial: 3->2->1.

	//Mientras estemos en un nivel de punteros indirectos.
	while(nivel_punteros > 0){ 
		//En el caso de que no hayan bloques que cuelguen del puntero...
		if(ptr == 0){
			/* Si reservar vale 0, usamos esta función solo para consultar.
			 * Por lo tanto solo devolvemos error.
			 * */
			if(reservar == '0'){ 
				
				printf("\ntraducir_bloque_inodo(): ERROR, no existe bloque físico para el bloque lógico %i.\n",blogico);
			
			}
			//Construimos la estructura...
			else { 
				//Indicamos que el inodo se ha de modificar y reescribir en el dispositivo.
				salvar_inodo = 1; 
				//Reservamos un bloque libre.	
				ptr = reservar_bloque();	
				if(ptr<0){
					
					printf("\ntraducir_bloque_inodo(): ERROR, no ha sido posible realizar la reserva de un bloque.\n");
				
					return -1;
				}
				//Llenamos el buffer de 0's para luego poder escribir en el dispositivo un bloque índice vacío.
				memset(bloque_indice,0,BLOCKSIZE/sizeof(int));
				//Como ptr!=0 podemos colgar un bloque de índices de él.
				bwrite(ptr,bloque_indice);
				inodo.numBloquesOcupados++;
				// Actualizamos el ctime
				localtime(&inodo.ctime);
				//Si el nuevo bloque de punteros cuelga directamente del inodo...
				if(rango == nivel_punteros){
					//Actualizamos el puntero correspondiente del inodo para que apunte a ptr.
					inodo.punterosIndirectos[rango-1] = ptr;
					
				}//Sino el nuevo bloque de punteros cuelga directamente de otro bloque de punteros...
				else{
					//Leemos el bloque índice anterior.
					bread(ptr_ant,bloque_indice);
					//Ponemos el puntero en el índice del bloque de índices.
					bloque_indice[obtener_indice(ptr_ant,nivel_punteros)] = ptr;
					//Escribimos en el dispositivo el bloque modificado.
					bwrite(ptr_ant,bloque_indice);
				}
			}		
		}
		bread(ptr,bloque_indice);
		int indice = obtener_indice(ptr,nivel_punteros);
		ptr_ant = ptr;
		//Para saber cuál es el siguiente bloque.
		ptr = bloque_indice[indice];
		//Vamos construyendo los bloques intermedios y sus enlaces desde el inodo hasta los bloques de datos.
		nivel_punteros--;
	}
	//Trata del nivel_punteros = 1 --> bloques de datos.
	if(ptr == 0){//Si no cuelga del bloque índice nada...
		
		if(reservar == '0'){
			
			printf("\ntraducir_bloque_inodo(): ERROR, no existe bloque físico para el bloque lógico %i.\n",blogico);
			
			return -1;
		}
		else{
			//Actualizaremos el inodo.
			salvar_inodo = 1;
			//Reservamos un bloque libre.		
			ptr = reservar_bloque();	
			if(ptr<0){
					
				printf("\ntraducir_bloque_inodo(): ERROR, no ha sido posible realizar la reserva de un bloque de datos.\n");
				
				return -1;
			}
			
			inodo.numBloquesOcupados++;
			// Actualizamos el ctime
			localtime(&inodo.ctime);
			
			//Si es un bloque de datos que cuelga de un puntero directo...
			if(rango == 0){
				
				inodo.punterosDirectos[blogico] = ptr;
				
			}//Sino es un bloque de datos que cuelga de un bloque de índices.
			else{
				
				//Leemos el bloque índice anterior.
				bread(ptr_ant,bloque_indice);
				//Ponemos el puntero en el índice del bloque de índices.
				bloque_indice[obtener_indice(ptr_ant,nivel_punteros)] = ptr;
				//Escribimos en el dispositivo el bloque modificado.
				bwrite(ptr_ant,bloque_indice);
						
			}
			
		}
	
	}
	
	//Si hay que reescribir el inodo...
	if(salvar_inodo == 1)escribir_inodo(inodo,ninodo);
		
	//Devolvemos el nº de bloque físico del bloque lógico parametrizado.
	
	return ptr;
}


/* Función liberar_bloques_inodo:Se encarga de eliminar todos los bloques ocupados del inodo
 * a partir del bloque lógico parametrizado inclusive hasta el último bloque
 * lógico del inodo = (inodo.tamEnBytesLog/BLOCKSIZE). .
 * 
 * Parámetros: 
 * 			-ninodo		: # inodo al que borraremos los bloques.
 *			-blogico	: bloque lógico a partir del que hay que empezar a borrar.
 * 
 * */
int liberar_bloques_inodo(unsigned int ninodo,unsigned int blogico,int ptr,int rango,int nivel){
	
	unsigned int mascara[BLOCKSIZE];
	struct SUPERBLOQUE sb;
	struct INODO inodo;
	unsigned int ultimoBloqueInodo;
	unsigned int bloque_indice[BLOCKSIZE/sizeof(int)];
	int indice;
	//Leemos el Superbloque
	if(bread(posSB,&sb)<0){

		printf("\nliberar_bloques_inodo(): ERROR, no se ha leído el SUPERBLOQUE.\n");

		return -1;
	}
	//Leemos el inodo
	inodo = leer_inodo(ninodo);
	ultimoBloqueInodo = inodo.tamEnBytesLog/BLOCKSIZE;
	//Rellenamos mascara de 0's para saber posteriormente si quedan índices apuntando.
	memset(mascara,0,BLOCKSIZE/sizeof(int));//máscara
	
	//Caso trivial.
	if(blogico == ultimoBloqueInodo){
		//Escribimos el inodo actualizado
		escribir_inodo(inodo,ninodo);
		//Actualizamos el SUPERBLOQUE
		if(bwrite(posSB,&sb)<0){

			printf("\nreservar_inodo(): ERROR, no se ha actualizado el SUPERBLOQUE.\n");

			return -1;
		}
		
		return ninodo;
	}
	
	//Para los punteros directos borramos el bloque de datos directamente...
	if(rango == 0){
		indice = obtener_indice(blogico,0);
		//Liberamos el bloque de datos
		liberar_bloque(inodo.punterosDirectos[indice]);
		//Tenemos un bloque libre más.
		sb.cantBloquesLibres++;
		//Ponemos a 0 el puntero directo al bloque de datos
		inodo.punterosDirectos[indice] = 0;
		//Decrementamos el nº de bloques ocupados
		inodo.numBloquesOcupados--;
		//Actualizamos el ctime
		localtime(&inodo.ctime);
		//Pasamos al siguiente bloque lógico
		blogico++;
		//Para saber si hay que saltar a otro rango.
		rango = obtener_rangoBL(inodo,blogico,&ptr);
		//Obtenemos el indice, en este caso ptr = indice.
		indice = obtener_indice(blogico,0);
		/* En el caso de los punteros directos tenemos el rango 0, pero
		 * si resulta que ya el rango no es cero sino 1, ya hay un nivel 
		 * de bloques índices.
		 * */
		if(rango == 1){
			nivel = rango;
			indice = obtener_indice(blogico,nivel);
		}
		//Liberamos el siguiente bloque.
		if(liberar_bloques_inodo(ninodo,blogico,ptr,rango,nivel)<0){
			printf("\nliberar_bloques_inodo(): ERROR, ha ocurrido un problema al intentar borrar el bloque %i.\n",blogico);
			return -1;
		}
	}
	
	//Para los punteros indirectos tratamos los bloques índice...
	//Si no estamos a nivel de datos...
	if(nivel!=0){
		//Si el bloque índice existe...
		if(ptr>0){
			//Leemos el bloque de punteros.
			if(bread(ptr,bloque_indice)<0){
				
				printf("\nliberar_bloques_inodo(): ERROR, no se ha podido leer un bloque índice.\n");		
							
				return -1;
			}
			//Obtenemos el índice en ese bloque.
			indice = obtener_indice(ptr,nivel);

			if(nivel == 3){
				//Para cambiar de nivel.
				nivel--;
				int bloque_indice_3 = ptr;
				//Calculamos el ptr siguiente.
				ptr = bloque_indice[indice];
			    if(liberar_bloques_inodo(ninodo,blogico,ptr,rango,nivel)<0){
					printf("\nliberar_bloques_inodo(): ERROR, ha ocurrido un problema al intentar borrar el bloque %i.\n",blogico);
					return -1;
				}
				//Ahora borrar hacia atrás según rango.
				nivel++;
				//Ponemos a 0 el puntero al bloque de índices de nivel 2.
				bloque_indice[indice] = 0;
				//Si no quedan punteros ocupados en el bloque índice de nivel 3
				if(memcmp(bloque_indice,mascara,BLOCKSIZE/sizeof(int)) == 0){
					if(eliminar_bloque_indice(nivel,ninodo,&sb,&inodo,ptr,&rango,bloque_indice_3,blogico)<0){
						printf("\nliberar_bloques_inodo(): ERROR, no se ha podido eliminar el bloque índice.\n");
						return -1;
					}
				}else{
					if(preservar_bloque(blogico,bloque_indice,ptr,ninodo,rango,nivel)<0){
						
						printf("\nliberar_bloques_inodo(): ERROR, no se ha podido preservar el bloque índice.\n");
						return -1;		
					}
				}

			}
			
			if(nivel == 2){
				//Para cambiar de nivel.
				nivel--;
				int bloque_indice_2 = ptr;
				//Calculamos el ptr siguiente.
				ptr = bloque_indice[indice];
			    if(liberar_bloques_inodo(ninodo,blogico,ptr,rango,nivel)<0){
					
					printf("\nliberar_bloques_inodo(): ERROR, ha ocurrido un problema al intentar borrar el bloque %i.\n",blogico);
					return -1;
				}
				//Ahora borrar hacia atrás según rango.
				nivel++;
				//Ponemos a 0 el puntero al bloque de índices de nivel 1.
				bloque_indice[indice] = 0;
				//Si no quedan punteros ocupados en el bloque índice de nivel 2
				if(memcmp(bloque_indice,mascara,BLOCKSIZE/sizeof(int)) == 0){
					
					if(eliminar_bloque_indice(nivel,ninodo,&sb,&inodo,ptr,&rango,bloque_indice_2,blogico)<0){
						printf("\nliberar_bloques_inodo(): ERROR, no se ha podido eliminar el bloque índice.\n");
						return -1;
					}
				}else{
					if(preservar_bloque(blogico,bloque_indice,ptr,ninodo,rango,nivel)<0){
						
						printf("\nliberar_bloques_inodo(): ERROR, no se ha podido preservar el bloque índice.\n");
						return -1;		
					}
				}
			}
			
			if(nivel == 1){
				//Tratamos el bloque de datos
				if(bloque_indice[indice]>0){//Existe el bloque de datos
					//Liberamos el bloque de datos
					liberar_bloque(bloque_indice[indice]);
					//Ponemos a 0 el puntero directo al bloque de datos
					bloque_indice[indice] = 0;
					//Tenemos un bloque libre más.
					sb.cantBloquesLibres++;
					//Decrementamos el nº de bloques ocupados
					inodo.numBloquesOcupados--;
					//Actualizamos el ctime
					localtime(&inodo.ctime);
				}
				//Si no quedan punteros ocupados en el bloque índice de nivel 1
				if(memcmp(bloque_indice,mascara,BLOCKSIZE/sizeof(int)) == 0){
					if(eliminar_bloque_indice(nivel,ninodo,&sb,&inodo,ptr,&rango,ptr,blogico)<0){
						
						printf("\nliberar_bloques_inodo(): ERROR, no se ha podido eliminar el bloque índice.\n");
						return -1;
					}

				}else{
					if(preservar_bloque(blogico,bloque_indice,ptr,ninodo,rango,nivel)<0){
						printf("\nliberar_bloques_inodo(): ERROR, no se ha podido preservar el bloque índice.\n");
						return -1;		
					}
				}
			}
			
		}else{
			
			printf("\nliberar_bloques_inodo(): ERROR, no existe el bloque a acceder.\n");
			
			return -1;
		}
	}
	
	return 0;
}
/* Función eliminar_bloque_indice:Se encarga de eliminar el bloque índice.
 * En cada condicional se encarga de cambiar el rango si es necesario y poner
 * el puntero del inodo al bloque indice de nivel 3, 2 o 1 a cero.
 * 
 * Parámetros:(Provienen de la función liberar_bloques_inodo().)
 * 
 * */
int eliminar_bloque_indice(int nivel,unsigned int ninodo,struct SUPERBLOQUE *sb,struct INODO *inodo, int ptr, int *rango,int ptr_ant,unsigned int blogico){
	
		//Liberamos el bloque de punteros del nivel que toque.
		liberar_bloque(ptr_ant);
		//Decrementamos el nº de bloques ocupados.
		(*inodo).numBloquesOcupados--;
		(*sb).cantBloquesLibres++;
		//Actualizamos el inodo.
		localtime(&(*inodo).ctime);
		if(nivel == 3){
			if(*rango == 3){
			//Poner a 0 el puntero del inodo a ese bloque.
			(*inodo).punterosIndirectos[*rango-1] = 0;
			blogico++;
				//Llamamos a la función y caso trivial.
				if(liberar_bloques_inodo(ninodo,blogico,ptr,*rango,nivel)<0){
					printf("\neliminar_bloque_indice(): ERROR, ha ocurrido un problema al intentar borrar el bloque %i.\n",blogico);
					return -1;
				}
			}
		}
		else if (nivel == 2){
			if(*rango == 2){
				//Poner a 0 el puntero del inodo a ese bloque.
				(*inodo).punterosIndirectos[*rango-1] = 0;
				*rango = obtener_rangoBL(*inodo,blogico+1,&ptr);
				if(*rango == 3)nivel = *rango;
					//Llamamos a la función y caso trivial.
					if(liberar_bloques_inodo(ninodo,blogico+1,ptr,*rango,nivel)<0){
						printf("\neliminar_bloque_indice(): ERROR, ha ocurrido un problema al intentar borrar el bloque %i.\n",blogico);
						return -1;
					}
				}
		}
		else if(nivel == 1){
			if(*rango == 1){
				(*inodo).punterosIndirectos[*rango-1] = 0;
				//Si tenemos que cambiar de rango...
				*rango = obtener_rangoBL(*inodo,blogico+1,&ptr);
				if(*rango == 2)nivel = *rango;
					//Llamamos a la función.
					if(liberar_bloques_inodo(ninodo,blogico+1,ptr,*rango,nivel)<0){
						printf("\neliminar_bloque_indice(): ERROR, ha ocurrido un problema al intentar borrar el bloque %i.\n",blogico);
						return -1;
					}
						
				}
		}
	
	return 0;
}

/* Función preservar_bloque:Se encarga de preservar el bloque de índices
 * y en caso de que rango = nivel, es decir, ya se pueda volver a liberar
 * otro bloque, se llama a la función liberar_bloques_inodo().
 * 
 * Parámetros:(Provienen de la función liberar_bloques_inodo().)
 * 
 * */
int preservar_bloque(unsigned int blogico,unsigned int *bloque_indice, int ptr, unsigned int ninodo,int rango,int nivel){
	//Preservamos el bloque de índices.
	if(bwrite(ptr,bloque_indice)<0){
						
		printf("\nliberar_bloques_inodo(): ERROR, no se ha podido escribir un bloque índice.\n");		
							
		return -1;
	}
	if(rango == nivel){
		//Liberamos el siguiente bloque...
		if(liberar_bloques_inodo(ninodo,blogico+1,ptr,rango,nivel)<0){
			printf("\nliberar_bloques_inodo(): ERROR, ha ocurrido un problema al intentar borrar el bloque %i.\n",blogico);
			return -1;
		}
	}
	return 0;
}

/* Función eliminar_inodo:Se encarga de eliminar todos los bloques que 
 * componen al inodo. Retorna el nº de inodo liberado.
 * 
 * Parámetros:
 * 			  -ninodo: # de inodo a borrar. 	
 * */
int liberar_inodo(unsigned int ninodo){
	
	struct INODO inodo;
	struct SUPERBLOQUE sb;
	int ptr;
	int nivel,blogico,rangoBL;
	
	//Leemos el inodo para liberarlo...
	inodo = leer_inodo(ninodo);
	//Borramos el inodo completamente.
	blogico = 0;
	nivel = 0;
	rangoBL = obtener_rangoBL(inodo,blogico,&ptr);

	if(liberar_bloques_inodo(ninodo,blogico,ptr,rangoBL,nivel)<0){
		
		printf("\nliberar_bloques_inodo(): ERROR, ha ocurrido un problema al liberar el inodo %i.\n",ninodo);
		
		return -1;
	}
	//Leemos el inodo actualizado.
	inodo = leer_inodo(ninodo);
	inodo.tipo = 'l';
	//Leemos el SUPERBLOQUE.
	if(bread(posSB,&sb)<0){

		printf("\nliberar_bloques_inodo(): ERROR, no se ha leído el SUPERBLOQUE.\n");

		return -1;
	}
	sb.posPrimerInodoLibre = ninodo;
	sb.cantInodosLibres++;
	//Actualizamos el SUPERBLOQUE y el INODO
	escribir_inodo(inodo,ninodo);
	if(bwrite(posSB,&sb)<0){

		printf("\nreservar_inodo(): ERROR, no se ha actualizado el SUPERBLOQUE.\n");

		return -1;
	}
	
	return ninodo;
}

