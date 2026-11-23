/* ============================================================================
 *  libros.h  -  Gestion de datos (libros) y su persistencia
 * ========================================================================== */
#ifndef LIBROS_H
#define LIBROS_H

#include "config.h"

/* Un libro de la biblioteca. 'owner' indica a que usuario pertenece. */
typedef struct {
    char owner[LEN_USER];
    char titulo[LEN_TITULO];
    char autor[LEN_AUTOR];
    int  anio;
    char genero[LEN_GENERO];
    int  leido;                 /* 0 = pendiente, 1 = leido */
} Libro;

/* Carga la biblioteca desde el fichero (al arrancar). */
void libros_cargar(void);

/* Operaciones interactivas sobre los libros del usuario 'owner'. */
void libros_agregar(const char *owner);
void libros_listar(const char *owner);
void libros_buscar(const char *owner);
void libros_modificar(const char *owner);
void libros_eliminar(const char *owner);

/* Rellena 'destino' con punteros a los libros de 'owner'.
 * Devuelve cuantos ha encontrado (lo usa el modulo de procesamiento). */
int  libros_obtener_de(const char *owner, Libro *destino[], int max);

/* Imprime una fila (lo reutiliza el modulo de procesamiento). */
void libros_imprimir_cabecera(void);
void libros_imprimir_fila(int id, const Libro *L);

#endif /* LIBROS_H */
