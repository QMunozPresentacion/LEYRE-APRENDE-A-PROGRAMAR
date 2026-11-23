# ============================================================================
#  Makefile - Gestor de Biblioteca Personal (SSI, Primera Parte)
#
#  El binario del profesor es de 32 bits, por lo que ESTE programa se compila
#  en 64 bits (sin -m32).
#
#  NOTA: cuando se introduzca la vulnerabilidad (ret2win), habra que anadir a
#  CFLAGS las opciones:  -fno-stack-protector -no-pie -z execstack
#  De momento, version limpia y sin vulnerabilidad.
# ============================================================================

CC      = gcc
CFLAGS  = -g -Wall
TARGET  = biblioteca
OBJ     = main.o ui.o usuarios.o libros.o procesamiento.o menus.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Cada .o depende de su .c; las cabeceras comunes fuerzan recompilar si cambian.
main.o:          main.c          ui.h usuarios.h libros.h menus.h config.h
ui.o:            ui.c            ui.h
usuarios.o:      usuarios.c      usuarios.h ui.h config.h
libros.o:        libros.c        libros.h ui.h config.h
procesamiento.o: procesamiento.c procesamiento.h libros.h ui.h config.h
menus.o:         menus.c         menus.h ui.h usuarios.h libros.h procesamiento.h

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
