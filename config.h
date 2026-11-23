/* ============================================================================
 *  config.h  -  Constantes globales del proyecto
 *  Reunir aqui los limites y los nombres de fichero evita repetirlos en cada
 *  modulo y facilita cambiarlos en un solo sitio.
 * ========================================================================== */
#ifndef CONFIG_H
#define CONFIG_H

/* Limites de almacenamiento en memoria */
#define MAX_USUARIOS   200
#define MAX_LIBROS     500

/* Longitudes maximas de las cadenas */
#define LEN_USER        50
#define LEN_PASS        50
#define LEN_TITULO     100
#define LEN_AUTOR      100
#define LEN_GENERO      50
#define LEN_LINEA      512

/* Ficheros de persistencia */
#define FICHERO_USUARIOS  "usuarios.txt"
#define FICHERO_LIBROS    "biblioteca.dat"

#endif /* CONFIG_H */
