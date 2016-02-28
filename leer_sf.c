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
	unsigned char mb[BLOCKSIZE];
	struct INODO inodos[BLOCKSIZE/T_INODO];
	int j;

	printf("\nleer_sf: Lectura del SUPERBLOQUE...\n\n");

	if(bread(posSB,&sb)<0)return -1;
	printf("posPrimerBloqueMB: %i\n",sb.posPrimerBloqueMB);
	printf("posPrimerBloqueAI: %i\n",sb.posPrimerBloqueAI);
	printf("posPrimerBloqueDatos: %i\n",sb.posPrimerBloqueDatos);
	printf("posInodoRaiz: %i\n",sb.posInodoRaiz);
	printf("posPrimerInodoLibre: %i\n",sb.posPrimerInodoLibre);
	printf("cantInodosLibres: %i\n",sb.cantInodosLibres);
	printf("totBloques: %i\n",sb.totBloques);
	printf("totInodos: %i\n",sb.totInodos);

	printf("\nleer_sf: Leyendo el MAPA DE BITS...\n");

	printf("MAPA DE BITS:\n");
	j=0;
	for (int i = 0; i < sb.totBloques; ++i){
		
		printf("%i",leer_bit(i));
		
		if(j == (BLOCKSIZE*8)){//Para que escriba en bloques el tamaño del MAPA DE BITS.
			printf("\n\n");
			j=0;
		}
		
		j++;
	}

	printf("\nleer_sf: Leyendo el ARRAY DE INODOS...\n");

	j=0;
	for (int i = 0; i < sb.totInodos; ++i){
		
		inodos[j]= leer_inodo(i);

		if(j == (BLOCKSIZE/T_INODO) -1){

			for (int k = 0; k < (BLOCKSIZE/T_INODO); ++k){

				printf("\nNº INODO %i:\n",i);

				printf("Tipo: %c\n",inodos[k].tipo);
				printf("Permisos: %i\n",inodos[k].permisos);
				printf("Nº enlaces: %i\n",inodos[k].nlinks);
				printf("Bytes lógicos: %i\n",inodos[k].tamEnBytesLog);
				printf("Bloques ocupados: %i\n",inodos[k].numBloquesOcupados);

			}
			
			printf("\n");

			j=-1;
		}

		j++;
	}

	printf("\nleer_sf: Desmontando el dispositivo...\n\n");

	//Desmontamos el dispositivo.
	printf("\nleer_sf: Desmontando el dispositivo.\n");
	bumount();
	printf("\nleer_sf: Dispositivo desmontado con éxito.\n");


	return 0;
}