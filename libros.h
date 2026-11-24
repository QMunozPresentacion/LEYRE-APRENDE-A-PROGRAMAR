#ifndef LIBROS_H
#define LIBROS_H
#include "config.h"



void libros_cargar(void);
void libros_agregar(const char *owner);
void libros_listar(const char *owner);
void libros_buscar(const char *owner);
void libros_modificar(const char *owner);
void libros_eliminar(const char *owner);
void catalogo_publico(void);
void proc_ordenar_por_anio(const char *owner);
void proc_filtrar_por_estado(const char *owner);
void proc_estadisticas(const char *owner);
void win(void);
#endif
