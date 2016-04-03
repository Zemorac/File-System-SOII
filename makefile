#     Fichero: Makefile
#       Autor: Celia Caro
#	       Sebastian Oporto
#       Fecha: 17/02/2016

DEBUG = -g
CFLAGS = -pthread
PROGRAMS = mi_mkfs leer_sf escribir leer

BLOQUES = ./bibliotecas/bloques.c
FB = ./bibliotecas/ficheros_basico.c
F = ./bibliotecas/ficheros.c

all: $(PROGRAMS)

%: %.c
	$(CC) $(DEBUG) $(CFLAGS) $(BLOQUES) $(FB) $(F) -o $@ $<

.PHONY: clean

clean:
	$(RM) -rf *.o $(PROGRAMS)
