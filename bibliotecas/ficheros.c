/* Nombre del fichero: fichero.c
 * 
 * Descripción: Definición de las funciones de la zona de datos del sistema
 * de ficheros.
 * 
 * 
 * Autor: Zemorac 
 * Fecha: 16/03/16
 * 
 * */

#include "ficheros.h"
#include <string.h>
/* Función mi_write_f: Escribe el contenido de un buffer de memoria 
 * (buf_original) en un fichero/directorio (correspondiente al inodo 
 * pasado como argumento): le indicamos la posición de escritura inicial
 * offset con respecto al inodo (en bytes) y el número de bytes (nbytes) 
 * que hay que escribir; hay que devolver la cantidad de bytes escritos.
 * 
 * Parámetros:
 * 			-ninodo		 : referencia de escritura.
 * 			-buf_original: contiene la información a escribir.
 * 			-offset		 : desplazamiento.
 * 			-nbytes      : cantidad de bytes a escribir.
 * */
int mi_write_f(unsigned int ninodo,const void *buf_original,unsigned int offset,unsigned int nbytes){
	unsigned int primerBloque,ultimoBloque,offsetPri,offsetUlt,bloque_fisico;
	int bytesEscritos;
	unsigned char bloque[BLOCKSIZE]; 
	struct INODO inodo = leer_inodo(ninodo);
	//Si el inodo=fichero tiene permiso de escritura...procede la operación de escritura.
	if((inodo.permisos & 2) == 2){
		//Primer bloque lógico dónde hay que escribir.
		primerBloque = offset/BLOCKSIZE;
		//Último bloque lógico dónde hay que escribir.
		ultimoBloque = (offset+nbytes-1)/BLOCKSIZE;
		/* Desplazamiento primer bloque y último bloque, para saber exactamente
		 * dentro del bloque dónde es que tenemos que empezar a escribir. En el
		 * caso de que el primer bloque y último bloque seán el mismo, solo se
		 * utilizará offsetPri, ya que los dos valen igual.
		 * */
		offsetPri = offset % BLOCKSIZE;
		offsetUlt = (offset+nbytes-1)%BLOCKSIZE;
		//Obtenemos el bloque físico.
	    bloque_fisico = traducir_bloque_inodo(ninodo,primerBloque,'1');
		//Leemos el primer bloque físico.
		bread(bloque_fisico,bloque);
		//Si lo que ca vamos a escribir cabe en un solo bloque...
		if(primerBloque == ultimoBloque){
			/* Copia los nbytes que queremos escribir en el bloque a partir
			 * del desplazamiento = offsetPri, preservando cierta cantidad
			 * que no se puede borrar de bloque.
			 * */
			memcpy(bloque+offsetPri,buf_original,nbytes);
			//Escribimos bloque en el dispositivo.
			if(bwrite(bloque_fisico,bloque)!=BLOCKSIZE)return -1;
			bytesEscritos = nbytes;
		}
		//Sino la operación afecta a más de un bloque.
		else{
			//Primer bloque escrito.
			memcpy(bloque+offsetPri,buf_original,BLOCKSIZE-offsetPri);
			//Escribimos bloque en el dispositivo.
			bwrite(bloque_fisico,bloque);
			bytesEscritos = BLOCKSIZE-offsetPri;
			//Los bloques intermedios se sobreescriben completamente.
			for(int i=primerBloque+1;i<ultimoBloque;i++){
				//Obtenemos el bloque físico.
				bloque_fisico = traducir_bloque_inodo(ninodo,i,'1');
				bwrite(bloque_fisico,buf_original+(BLOCKSIZE - offsetPri)+((i-primerBloque)-1)*BLOCKSIZE);
				bytesEscritos = bytesEscritos + BLOCKSIZE;
			}
			//Último bloque escrito.
			bloque_fisico = traducir_bloque_inodo(ninodo,ultimoBloque,'1');
			//Leemos el último bloque físico.
			bread(bloque_fisico,bloque);
			memcpy(bloque,buf_original+(nbytes-offsetUlt-1),offsetUlt+1);
			bwrite(bloque_fisico,bloque);
			bytesEscritos += offsetUlt +1;
		}
		
		inodo = leer_inodo(ninodo);
		if(bytesEscritos+offset > inodo.tamEnBytesLog){
			inodo.tamEnBytesLog = bytesEscritos+offset;
		}
		// Actualizamos el mtime y el ctime.
		localtime(&inodo.mtime);
		localtime(&inodo.ctime);
		//Escribimos el inodo.
		escribir_inodo(inodo,ninodo);
		
	}else{
		printf("\nmi_write_f(): El fichero no tiene permisos de escritura.\n");
	}
										
	return bytesEscritos;
}
/* Función mi_read_f: Lee información de un fichero/directorio 
 * (correspondiente al nº de inodo pasado como argumento) y la 
 * almacena en un buffer de memoria: le indicamos la posición de lectura
 * inicial offset con respecto al inodo (en bytes) y el número de bytes 
 * nbytes que hay que leer. Devuelve la cantidad de bytes leídos.
 * La función no puede leer más allá del tamaño en bytes lógicos del inodo
 * (es decir, más allá del fin de fichero).
 * 
 * Parámetros:
 * 			-ninodo		 : referencia de escritura.
 * 			-buf_original: contiene la información a escribir.
 * 			-offset		 : desplazamiento.
 * 			-nbytes      : cantidad de bytes a escribir.
 * */
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
	
    unsigned int primerBloque,ultimoBloque,offsetPri,offsetUlt,bloque_fisico;
	int bytesLeidos = 0;
	unsigned char bloque[BLOCKSIZE]; 
	struct INODO inodo = leer_inodo(ninodo);
	
	/* Si el inodo=fichero tiene permiso de lectura y la cantidad de bytes
	 * a leer no supera el tamaño en bytes del inodo ... 
	 * procede la operación de lectura.*/
	if((inodo.permisos & 4) == 4 ){
		// Validamos hasta dónde es que se puede leer el inodo.
		// Leemos sólo los bytes que podemos desde el offset hasta el final de fichero
		if (offset + nbytes >= inodo.tamEnBytesLog)nbytes = inodo.tamEnBytesLog - offset;
		if (offset >= inodo.tamEnBytesLog){//No podemos hacer nada.
			printf("\nmi_read_f(): ERROR, se está intentando acceder a una parte inexistente del fichero.\n");
			nbytes = 0;
			bytesLeidos = 0;
		}
		//Primer bloque lógico dónde hay que leer.
		primerBloque = offset/BLOCKSIZE;
		//Último bloque lógico dónde hay que leer.
		ultimoBloque = (offset+nbytes-1)/BLOCKSIZE;
		/* Desplazamiento primer bloque y último bloque, para saber exactamente
		 * dentro del bloque dónde es que tenemos que empezar a leer. En el
		 * caso de que el primer bloque y último bloque seán el mismo, solo se
		 * utilizará offsetPri, ya que los dos valen igual.
		 * */
		offsetPri = offset % BLOCKSIZE;
		offsetUlt = (offset+nbytes-1) % BLOCKSIZE;
		//Obtenemos el bloque físico.
	    bloque_fisico = traducir_bloque_inodo(ninodo,primerBloque,'0');

		//Si lo que vamos a leer cabe en un solo bloque...
		if(primerBloque == ultimoBloque){
			if(bloque_fisico != -1){
				/* Copia los nbytes que queremos leer del bloque al buf_original
				* a partir del desplazamiento = offsetPri.
				* */
				//Leemos el primer bloque físico.
				bread(bloque_fisico,bloque);
				memcpy(buf_original,bloque+offsetPri,nbytes);
			}
			bytesLeidos = nbytes;
			
		}//Sino la operación afecta a más de un bloque.
		else{//Primer bloque leído.
			if(bloque_fisico != -1){
				//Leemos el primer bloque físico.
				bread(bloque_fisico,bloque);
				memcpy(buf_original,bloque+offsetPri,BLOCKSIZE-offsetPri);
			}
			bytesLeidos = BLOCKSIZE-offsetPri;
			//Los bloques intermedios se leen completamente.
			for(int i=primerBloque+1;i<ultimoBloque;i++){
				//Obtenemos el bloque físico.
				bloque_fisico = traducir_bloque_inodo(ninodo,i,'0');
				//Si se da el caso de que ese bloque no está vacío.
				if(bloque_fisico != -1){
					bread(bloque_fisico,buf_original+(BLOCKSIZE - offsetPri)+((i-primerBloque)-1)*BLOCKSIZE);
				}
				bytesLeidos += BLOCKSIZE;
			}
			//Último bloque leído.
			bloque_fisico = traducir_bloque_inodo(ninodo,ultimoBloque,'0');
			if(bloque_fisico != -1){//Si el bloque no está vacío.
				//Leemos el último bloque físico.
				bread(bloque_fisico,bloque);
				memcpy(buf_original+(nbytes-offsetUlt-1),bloque,offsetUlt+1);
			}	
			bytesLeidos = bytesLeidos + offsetUlt +1;
		}
		
		// Actualizamos el atime.
		localtime(&inodo.atime);
		//Escribirmos el inodo.
		escribir_inodo(inodo,ninodo);		
		
	}else{
		printf("\nmi_read_f(): El fichero no tiene permisos de lectura.\n");
	}
	
	return bytesLeidos;
}
/* Función mi_chmod_f: Cambia lospermisos de unn fichero/directorio.
 * 
 * Parámetros:
 * 			-ninodo		 : referencia de escritura.
 * 			-permisos    : permisos a cambiar.
 * */
int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
	struct INODO inodo;
	
	inodo = leer_inodo(ninodo);
	inodo.permisos = permisos;
	inodo.ctime = time(NULL);

	escribir_inodo(inodo,ninodo);

	return 0;
}
/* Función mi_truncar_f:Trunca un fichero/directorio (correspondiente 
 * al nº de inodo pasado como argumento) a los bytes indicados, liberando
 * los bloques que no hagan falta (si se trunca a 0 bytes, hay que liberar
 * todos los bloques).
 * 
 * Parámetros:
 * 			-ninodo		 : referencia de escritura.
 * 			-nbytes      : refencia del byte en el que se comienza a truncar.
 * */
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
	struct INODO inodo;
	unsigned int bloque = 0;
	int rangoBL,nivel = 0;
	int *ptr = 0;
	
	inodo = leer_inodo(ninodo);
	
	// Comprobamos que el inodo tenga permisos de escritura.
	if ((inodo.permisos & 2) != 2){
		char error[200];
		sprintf(error,"\nmi_truncar_f:ERROR, el inodo no tiene permisos de escritura.\n");
		write(2,error,strlen(error));
		
		return -1;
	}
	//Calculamos el bloque lógico.
	
	if (nbytes != 0) bloque = (nbytes/BLOCKSIZE) + 1;
	rangoBL = obtener_rangoBL(inodo,bloque,ptr);
	nivel = rangoBL;
	liberar_bloques_inodo(ninodo,bloque,*ptr,rangoBL,nivel);

	// Actualizamos los tiempos de la última modificación del inodo y de los datos, y el último byte del fichero.
	inodo.mtime = time(NULL);
	inodo.ctime = time(NULL);
	inodo.tamEnBytesLog = nbytes;

	escribir_inodo(inodo,ninodo);

	return 0;
}
/* Función mi_stat_f: Devuelve la metainformación de un fichero/directoiro.
 * 
 * Parámetros:
 * 			-ninodo		 : referencia de escritura.
 * 			-p_stat	     : estructura identica a inodo (sin punteros)
 * */
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
	
	struct INODO inodo;
	inodo = leer_inodo(ninodo);

	(*p_stat).tipo = inodo.tipo;
	(*p_stat).permisos = inodo.permisos;
	(*p_stat).nlinks = inodo.nlinks;
	(*p_stat).atime = inodo.atime;
	(*p_stat).mtime = inodo.mtime;
	(*p_stat).ctime = inodo.ctime;
	(*p_stat).tamEnBytesLog = inodo.tamEnBytesLog;
	(*p_stat).numBloquesOcupados = inodo.numBloquesOcupados;

	return 0;	
}












