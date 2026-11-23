/* ============================================================================
 *  ui.c  -  Implementacion de la interfaz de texto
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"

void limpiar_pantalla(void) {
    printf("\033[2J\033[H");
}

void pausar(void) {
    char tmp[16];
    printf(C_AMAR "\n  Pulsa ENTER para continuar..." C_RESET);
    fflush(stdout);
    fgets(tmp, sizeof(tmp), stdin);
}

void banner(void) {
    printf(C_CYAN);
    printf("  +==================================================+\n");
    printf("  |                                                  |\n");
    printf("  |      ___   BIBLIOTECA  PERSONAL   ___            |\n");
    printf("  |     |   |  ____________________   |   |          |\n");
    printf("  |     |[ ]| |  _  _  _  _  _  _  _ | |[ ]|          |\n");
    printf("  |     |   | | |_||_||_||_||_||_||_|| |   |          |\n");
    printf("  |     |___| |____________________| |___|          |\n");
    printf("  |                                                  |\n");
    printf("  |         Gestor de libros en C  -  SSI            |\n");
    printf("  |                                                  |\n");
    printf("  +==================================================+\n");
    printf(C_RESET);
}

void titulo_seccion(const char *texto) {
    printf(C_AZUL "\n  +----------------------------------------+\n");
    printf(       "  | " C_BLANCO "%-38s" C_AZUL " |\n", texto);
    printf(       "  +----------------------------------------+\n" C_RESET);
}

void msg_ok(const char *texto)    { printf(C_VERDE "  [OK] %s\n" C_RESET, texto); }
void msg_error(const char *texto) { printf(C_ROJO  "  [!!] %s\n" C_RESET, texto); }
void msg_info(const char *texto)  { printf(C_AMAR  "  [i]  %s\n" C_RESET, texto); }

int leer_linea(const char *prompt, char *destino, int tam) {
    if (prompt) { printf("%s", prompt); fflush(stdout); }
    if (fgets(destino, tam, stdin) == NULL) {
        destino[0] = '\0';
        return 0;
    }
    destino[strcspn(destino, "\n")] = '\0';   /* elimina el salto de linea */
    return 1;
}

int leer_entero(const char *prompt) {
    char buf[64];
    /* Si se cierra la entrada (EOF / Ctrl-D) lo tratamos como 0 = salir/volver,
     * para que el programa termine de forma ordenada y no entre en bucle. */
    if (!leer_linea(prompt, buf, sizeof(buf))) return 0;

    /* strtol nos permite comprobar que TODA la linea es un numero valido. */
    char *fin;
    long valor = strtol(buf, &fin, 10);
    if (fin == buf || *fin != '\0') return -1;   /* habia texto no numerico */
    return (int) valor;
}

int validar_campo(const char *s) {
    if (s == NULL || s[0] == '\0') return 0;
    if (strchr(s, '|') || strchr(s, ':')) return 0;  /* separadores reservados */
    return 1;
}
