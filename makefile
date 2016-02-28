#     Fichero: Makefile
#       Autor: Zemorac
#       Fecha: 17/02/2016

DEBUG = -g
CFLAGS = -pthread
PROGRAMS = mi_mkfs leer_sf

BLOQUES = ./bibliotecas/bloques.c
FB = ./bibliotecas/ficheros_basico.c

all: $(PROGRAMS)

%: %.c
	$(CC) $(DEBUG) $(CFLAGS) $(BLOQUES) $(FB) -o $@ $<

.PHONY: clean

clean:
	$(RM) -rf *.o $(PROGRAMS)
