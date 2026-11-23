/* ============================================================================
 *  libros.c  -  Implementacion de la gestion de datos (libros)
 *
 *  Formato del fichero biblioteca.dat (una linea por libro):
 *      owner|titulo|autor|anio|genero|leido
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "ui.h"
#include "libros.h"

/* Estado interno del modulo */
static Libro biblioteca[MAX_LIBROS];
static int   num_libros = 0;

/* ------------------------- Funciones auxiliares -------------------------- */

void libros_imprimir_cabecera(void) {
    printf(C_BLANCO "  %-3s %-30s %-20s %-6s %-14s %-9s\n" C_RESET,
           "ID", "Titulo", "Autor", "Anio", "Genero", "Estado");
    printf("  --------------------------------------------------------------------------------\n");
}

void libros_imprimir_fila(int id, const Libro *L) {
    printf("  %-3d %-30.30s %-20.20s %-6d %-14.14s %-9s\n",
           id, L->titulo, L->autor, L->anio, L->genero,
           L->leido ? "Leido" : "Pendiente");
}

/* Indices (en el array interno) de los libros de un usuario. */
static int indices_de(const char *owner, int idx[]) {
    int n = 0;
    for (int i = 0; i < num_libros; i++)
        if (strcmp(biblioteca[i].owner, owner) == 0)
            idx[n++] = i;
    return n;
}

/* Guarda toda la biblioteca en el fichero. Devuelve 1 si tuvo exito. */
static int guardar_biblioteca(void) {
    FILE *f = fopen(FICHERO_LIBROS, "w");
    if (f == NULL) {
        msg_error("No se pudo escribir el fichero de la biblioteca.");
        return 0;
    }
    for (int i = 0; i < num_libros; i++) {
        Libro *L = &biblioteca[i];
        fprintf(f, "%s|%s|%s|%d|%s|%d\n",
                L->owner, L->titulo, L->autor, L->anio, L->genero, L->leido);
    }
    fclose(f);
    return 1;
}

/* ----------------------------- Interfaz publica -------------------------- */

void libros_cargar(void) {
    num_libros = 0;
    FILE *f = fopen(FICHERO_LIBROS, "r");
    if (f == NULL) return;

    char linea[LEN_LINEA];
    while (fgets(linea, sizeof(linea), f) && num_libros < MAX_LIBROS) {
        linea[strcspn(linea, "\n")] = '\0';
        if (linea[0] == '\0') continue;

        Libro *L = &biblioteca[num_libros];
        char *campo;

        campo = strtok(linea, "|"); if (!campo) continue;
        strncpy(L->owner, campo, LEN_USER - 1);    L->owner[LEN_USER - 1]   = '\0';
        campo = strtok(NULL, "|"); if (!campo) continue;
        strncpy(L->titulo, campo, LEN_TITULO - 1); L->titulo[LEN_TITULO - 1] = '\0';
        campo = strtok(NULL, "|"); if (!campo) continue;
        strncpy(L->autor, campo, LEN_AUTOR - 1);   L->autor[LEN_AUTOR - 1]   = '\0';
        campo = strtok(NULL, "|"); if (!campo) continue;
        L->anio = atoi(campo);
        campo = strtok(NULL, "|"); if (!campo) continue;
        strncpy(L->genero, campo, LEN_GENERO - 1); L->genero[LEN_GENERO - 1] = '\0';
        campo = strtok(NULL, "|"); if (!campo) continue;
        L->leido = atoi(campo);

        num_libros++;
    }
    fclose(f);
}

void libros_agregar(const char *owner) {
    titulo_seccion("Anadir un libro");
    if (num_libros >= MAX_LIBROS) {
        msg_error("La biblioteca esta llena.");
        return;
    }

    Libro L;
    memset(&L, 0, sizeof(L));
    strncpy(L.owner, owner, LEN_USER - 1);

    leer_linea("  Titulo: ", L.titulo, sizeof(L.titulo));
    leer_linea("  Autor:  ", L.autor,  sizeof(L.autor));
    if (!validar_campo(L.titulo) || !validar_campo(L.autor)) {
        msg_error("Titulo o autor no validos.");
        return;
    }

    L.anio = leer_entero("  Anio de publicacion: ");
    if (L.anio < 0 || L.anio > 2100) {
        msg_error("Anio fuera de rango.");
        return;
    }

    leer_linea("  Genero: ", L.genero, sizeof(L.genero));
    if (!validar_campo(L.genero)) {
        msg_error("Genero no valido.");
        return;
    }

    int r = leer_entero("  Leido? (1 = si, 0 = no): ");
    L.leido = (r == 1) ? 1 : 0;

    biblioteca[num_libros++] = L;
    if (guardar_biblioteca())
        msg_ok("Libro anadido a tu biblioteca.");
}

void libros_listar(const char *owner) {
    titulo_seccion("Mis libros");
    int idx[MAX_LIBROS];
    int n = indices_de(owner, idx);
    if (n == 0) {
        msg_info("Todavia no tienes libros registrados.");
        return;
    }

    libros_imprimir_cabecera();
    for (int i = 0; i < n; i++)
        libros_imprimir_fila(i + 1, &biblioteca[idx[i]]);
    printf(C_CYAN "\n  Total: %d libro(s).\n" C_RESET, n);
}

void libros_buscar(const char *owner) {
    titulo_seccion("Buscar en mis libros");
    printf("  Criterios:  1) Titulo   2) Autor   3) Genero\n");
    int criterio = leer_entero("  Elige criterio: ");
    if (criterio < 1 || criterio > 3) {
        msg_error("Criterio no valido.");
        return;
    }

    char patron[LEN_TITULO];
    leer_linea("  Texto a buscar: ", patron, sizeof(patron));

    int idx[MAX_LIBROS];
    int n = indices_de(owner, idx);
    int encontrados = 0;

    libros_imprimir_cabecera();
    for (int i = 0; i < n; i++) {
        Libro *L = &biblioteca[idx[i]];
        const char *campo = (criterio == 1) ? L->titulo :
                            (criterio == 2) ? L->autor  : L->genero;
        if (strstr(campo, patron) != NULL) {
            libros_imprimir_fila(i + 1, L);
            encontrados++;
        }
    }
    if (encontrados == 0) msg_info("Ningun libro coincide con la busqueda.");
    else printf(C_CYAN "\n  %d coincidencia(s).\n" C_RESET, encontrados);
}

void libros_modificar(const char *owner) {
    libros_listar(owner);
    int idx[MAX_LIBROS];
    int n = indices_de(owner, idx);
    if (n == 0) return;

    int sel = leer_entero("\n  ID del libro a modificar (0 = cancelar): ");
    if (sel <= 0 || sel > n) {
        msg_info("Operacion cancelada.");
        return;
    }

    Libro *L = &biblioteca[idx[sel - 1]];
    char buf[LEN_TITULO];
    printf("  (Deja el campo vacio para mantener el valor actual)\n");

    leer_linea("  Nuevo titulo: ", buf, sizeof(buf));
    if (buf[0] && validar_campo(buf)) { strncpy(L->titulo, buf, LEN_TITULO - 1); L->titulo[LEN_TITULO - 1] = '\0'; }

    leer_linea("  Nuevo autor: ", buf, sizeof(buf));
    if (buf[0] && validar_campo(buf)) { strncpy(L->autor, buf, LEN_AUTOR - 1); L->autor[LEN_AUTOR - 1] = '\0'; }

    leer_linea("  Nuevo genero: ", buf, sizeof(buf));
    if (buf[0] && validar_campo(buf)) { strncpy(L->genero, buf, LEN_GENERO - 1); L->genero[LEN_GENERO - 1] = '\0'; }

    leer_linea("  Nuevo estado (1=leido, 0=pendiente, vacio=igual): ", buf, sizeof(buf));
    if (buf[0]) L->leido = (atoi(buf) == 1) ? 1 : 0;

    if (guardar_biblioteca())
        msg_ok("Libro modificado.");
}

void libros_eliminar(const char *owner) {
    libros_listar(owner);
    int idx[MAX_LIBROS];
    int n = indices_de(owner, idx);
    if (n == 0) return;

    int sel = leer_entero("\n  ID del libro a eliminar (0 = cancelar): ");
    if (sel <= 0 || sel > n) {
        msg_info("Operacion cancelada.");
        return;
    }

    int pos = idx[sel - 1];
    for (int i = pos; i < num_libros - 1; i++)   /* desplaza el resto */
        biblioteca[i] = biblioteca[i + 1];
    num_libros--;

    if (guardar_biblioteca())
        msg_ok("Libro eliminado.");
}

int libros_obtener_de(const char *owner, Libro *destino[], int max) {
    int n = 0;
    for (int i = 0; i < num_libros && n < max; i++)
        if (strcmp(biblioteca[i].owner, owner) == 0)
            destino[n++] = &biblioteca[i];
    return n;
}
