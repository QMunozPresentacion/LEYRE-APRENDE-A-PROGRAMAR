#ifndef GATOS_H
#define GATOS_H
#include "config.h"



void gatos_cargar(void);
void gatos_agregar(const char *owner);
void gatos_listar(const char *owner);
void gatos_buscar(const char *owner);
void gatos_modificar(const char *owner);
void gatos_eliminar(const char *owner);
void catalogo_publico(void);
void proc_ordenar_por_edad(const char *owner);
void proc_filtrar_por_estado(const char *owner);
void proc_estadisticas(const char *owner);
void win(void);
#endif
