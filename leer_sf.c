/* Nombre del fichero: leer_sf.c
 * 
 * Descripción: Lee el SUPERBLOQUE, MAPA DE BITS y ARRAY DE INODOS del
 * dispositivo (fichero) con el tamaño adecuado.
 * 
 * Autor: Zemorac 
 * Fecha: 17/02/16
 * 
 * */

#include "./bibliotecas/bloques.h"
#include "./bibliotecas/ficheros_basico.h"

int main(int argc, char **argv){

	if(argc!=2){
		
		printf("\nleer_sf: Error debe escribir: $ ./leer_sf <nombre_fichero> \n");
		
		return -1;
	}

	//Montamos el dispositivo.
	printf("\nleer_sf: Montando el dispositivo...\n");	
	bmount(argv[1]);//nombre_fichero
	printf("\nleer_sf: El dispositivo se ha montado con éxito.\n");

	struct SUPERBLOQUE sb;
	struct INODO inodo;
	struct tm *ts;
	char atime[80];
	char mtime[80];
	char ctime[80];

	printf("\nleer_sf: Lectura del SUPERBLOQUE...\n\n");

	if(bread(posSB,&sb)<0)return -1;
	printf("\n\n");
	printf("____________________________________________\n");
	printf("********** CAMPOS DEL SUPERBLOQUE **********\n\n");
	printf("Primer bloque MB: %i\n",sb.posPrimerBloqueMB);
	printf("Último bloque MB: %i\n",sb.posUltimoBloqueMB);
	printf("Primer bloque AI: %i\n",sb.posPrimerBloqueAI);	
	printf("Último bloque AI: %i\n",sb.posUltimoBloqueAI);	
	printf("Primer bloque datos: %i\n",sb.posPrimerBloqueDatos);	
	printf("Último bloque datos: %i\n",sb.posUltimoBloqueDatos);
	printf("Posición inodo raiz: %i\n",sb.posInodoRaiz);	
	printf("Posición primer inodo libre: %i\n",sb.posPrimerInodoLibre);
	printf("Número de inodos libres: %i\n",sb.cantInodosLibres);	
	printf("Número de bloques libres: %i\n",sb.cantBloquesLibres);
	printf("Número de bloques: %i\n",sb.totBloques);
	printf("Número de inodos: %i\n",sb.totInodos);
	printf("____________________________________________\n\n\n\n");
	
	printf("________________________________________________\n");
	printf("******************* TAMAÑOS ********************\n\n");
	printf("Tamaño en bloques del MAPA DE BITS: %i\n",tamMB(sb.totBloques));
	printf("Tamaño en bloques del ARRAY DE INODOS: %i\n",tamAI(sb.totInodos));
	printf("Tamaño en bytes de la estructura INODO: %ld\n",sizeof(inodo));
	printf("________________________________________________\n\n\n\n");

	printf("___________________________________________________________________________________________________________________________\n");
	printf("****************************************** BLOQUES OCUPADOS (MAPA DE BITS) ************************************************\n\n");
	int numBloque = 0;
	while(numBloque < sb.totBloques){
		if (leer_bit(numBloque)==1) printf("%d·",numBloque);
		numBloque++;
	}	
	printf("\n_________________________________________________________________________________________________________________________\n\n\n\n");

	
	printf("___________________________________________________________________________________\n");
	printf("*************************** INODOS OCUPADOS **************************\n\n");
	for (int i = 0; i < sb.totInodos; ++i){
		inodo = leer_inodo(i);
		if(inodo.tipo != 'l'){
			// Mostramos el número de inodo y su tipo.
			printf("---------- INODO Nº%d ----------\n",i);
			printf("Tipo: %c\n",inodo.tipo);
			// Mostramos los permisos.
			switch(inodo.permisos){
				case 7: printf("Permisos: rwx"); break;
				case 6: printf("Permisos: rw"); break;
				case 5: printf("Permisos: rx"); break;
				case 4: printf("Permisos: r"); break;
				case 3: printf("Permisos: wx"); break;
				case 2: printf("Permisos: w"); break;
				case 1: printf("Permisos: x"); break;
				case 0: printf("Permisos: Ninguno."); break;
			}
			// Mostramos el tamaño en bytes lógicos y la cantidad de bloques ocupados por este.
			printf("\nTamaño en bytes lógicos: %d\n",inodo.tamEnBytesLog);
			printf("Cantidad de bloques ocupados: %d\n",inodo.numBloquesOcupados);
			// Mostramos los sellos de tiempo del inodo.
			ts = localtime(&inodo.atime);
			strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S",ts);
			ts = localtime(&inodo.mtime);
			strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S",ts);
			ts = localtime(&inodo.ctime);
			strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S",ts);
			printf("ATIME (fecha y hora del último acceso a datos): %s\n",atime);
			printf("MTIME (fecha y hora de la última modificación de datos): %s\n",mtime);
			printf("CTIME (fecha y hora de la última modificación del inodo): %s\n",ctime);
			printf("\n");
		}		
	}
	printf("___________________________________________________________________________________\n\n\n\n");
	
	//Desmontamos el dispositivo.
	printf("\nleer_sf: Desmontando el dispositivo...\n\n");
	bumount();
	printf("\nleer_sf: Dispositivo desmontado con éxito.\n");


	return 0;
}
