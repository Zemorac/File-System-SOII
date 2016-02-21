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

	printf("\nleer_sf: Desmontando el dispositivo...\n\n");

	//Desmontamos el dispositivo.
	printf("\nleer_sf: Desmontando el dispositivo.\n");
	bumount();
	printf("\nleer_sf: Dispositivo desmontado con éxito.\n");


	return 0;
}