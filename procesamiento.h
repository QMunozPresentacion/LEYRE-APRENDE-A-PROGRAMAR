/* ============================================================================
 *  procesamiento.h  -  Operaciones sobre los datos almacenados
 * ========================================================================== */
#ifndef PROCESAMIENTO_H
#define PROCESAMIENTO_H

/* Las tres operaciones de procesamiento, sobre los libros de 'owner'. */
void proc_ordenar_por_anio(const char *owner);
void proc_filtrar_por_estado(const char *owner);
void proc_estadisticas(const char *owner);

#endif /* PROCESAMIENTO_H */
