/* ============================================================================
 *  procesamiento.c  -  Ordenacion, filtrado y estadisticas
 *
 *  Este modulo NO accede directamente al array interno de libros.c: pide los
 *  libros del usuario mediante libros_obtener_de() y trabaja sobre punteros.
 * ========================================================================== */
#include <stdio.h>
#include "config.h"
#include "ui.h"
#include "libros.h"
#include "procesamiento.h"

/* 1) Ordenar por anio (ordenacion burbuja sobre una lista de punteros). */
void proc_ordenar_por_anio(const char *owner) {
    titulo_seccion("Libros ordenados por anio");

    Libro *lista[MAX_LIBROS];
    int n = libros_obtener_de(owner, lista, MAX_LIBROS);
    if (n == 0) { msg_info("No hay libros."); return; }

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1 - i; j++)
            if (lista[j]->anio > lista[j + 1]->anio) {
                Libro *tmp = lista[j];
                lista[j] = lista[j + 1];
                lista[j + 1] = tmp;
            }

    libros_imprimir_cabecera();
    for (int i = 0; i < n; i++)
        libros_imprimir_fila(i + 1, lista[i]);
}

/* 2) Filtrar por estado (leidos / pendientes). */
void proc_filtrar_por_estado(const char *owner) {
    titulo_seccion("Filtrar por estado");
    int estado = leer_entero("  1) Leidos   0) Pendientes : ");
    if (estado != 0 && estado != 1) { msg_error("Opcion no valida."); return; }

    Libro *lista[MAX_LIBROS];
    int n = libros_obtener_de(owner, lista, MAX_LIBROS);
    int c = 0;

    libros_imprimir_cabecera();
    for (int i = 0; i < n; i++)
        if (lista[i]->leido == estado) {
            libros_imprimir_fila(c + 1, lista[i]);
            c++;
        }

    if (c == 0) msg_info("No hay libros en ese estado.");
    else printf(C_CYAN "\n  %d libro(s).\n" C_RESET, c);
}

/* 3) Estadisticas y resumen. */
void proc_estadisticas(const char *owner) {
    titulo_seccion("Estadisticas de mi biblioteca");

    Libro *lista[MAX_LIBROS];
    int n = libros_obtener_de(owner, lista, MAX_LIBROS);
    if (n == 0) { msg_info("No hay libros."); return; }

    int leidos = 0, pendientes = 0, anio_min = 99999, anio_max = -1;
    long suma_anios = 0;

    for (int i = 0; i < n; i++) {
        Libro *L = lista[i];
        if (L->leido) leidos++; else pendientes++;
        if (L->anio < anio_min) anio_min = L->anio;
        if (L->anio > anio_max) anio_max = L->anio;
        suma_anios += L->anio;
    }

    printf("  Total de libros .......... %d\n", n);
    printf("  Leidos ................... %d (%.1f%%)\n", leidos, 100.0 * leidos / n);
    printf("  Pendientes ............... %d (%.1f%%)\n", pendientes, 100.0 * pendientes / n);
    printf("  Anio mas antiguo ......... %d\n", anio_min);
    printf("  Anio mas reciente ........ %d\n", anio_max);
    printf("  Anio medio de publicacion  %.1f\n", (double) suma_anios / n);
}
