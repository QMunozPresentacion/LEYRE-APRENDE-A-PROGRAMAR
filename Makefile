# ============================================================================
#  Makefile - Gestor de Biblioteca Personal (SSI, Primera Parte)
#
#  El binario del profesor es de 32 bits, por lo que ESTE programa se compila
#  en 64 bits (sin -m32).
#
#  Flags de compilacion VULNERABLE (necesarias para el ret2win):
#    -fno-stack-protector : quita el canario de pila (si no, aborta el overflow)
#    -no-pie              : direcciones fijas  -> win() siempre en la misma
#    -z execstack         : pila "clasica" ejecutable
#    -g                   : simbolos de depuracion (para calcular el offset)
#    -w                   : silencia el warning del fgets vulnerable (a proposito)
# ============================================================================

CC      = gcc
CFLAGS  = -g -fno-stack-protector -no-pie -z execstack -w
TARGET  = biblioteca
OBJ     = main.o ui.o usuarios.o libros.o procesamiento.o catalogo.o menus.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Cada .o depende de su .c; las cabeceras comunes fuerzan recompilar si cambian.
main.o:          main.c          ui.h usuarios.h libros.h menus.h config.h
ui.o:            ui.c            ui.h
usuarios.o:      usuarios.c      usuarios.h ui.h config.h
libros.o:        libros.c        libros.h ui.h config.h
procesamiento.o: procesamiento.c procesamiento.h libros.h ui.h config.h
catalogo.o:      catalogo.c      catalogo.h ui.h
menus.o:         menus.c         menus.h ui.h usuarios.h libros.h procesamiento.h catalogo.h

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
