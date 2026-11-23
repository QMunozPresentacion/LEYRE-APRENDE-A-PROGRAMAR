/* ============================================================================
 *  catalogo.c  -  Catalogo publico  >>> MODULO CON LA VULNERABILIDAD <<<
 *
 *  Contiene las DOS piezas del ret2win:
 *    - win(): lee ./flag.txt y lo imprime. No se llama desde ningun menu.
 *    - catalogo_publico(): tiene el Buffer Overflow (accesible SIN login).
 *
 *  El resto del programa usa las lecturas SEGURAS de ui.c. Solo esta funcion
 *  es explotable, de modo que el exploit es determinista.
 * ========================================================================== */
#include <stdio.h>
#include <string.h>
#include "ui.h"
#include "catalogo.h"

/* -------------------------------------------------------------------------
 *  Funcion objetivo del exploit.
 *  Lee ./flag.txt y lo muestra. NO se referencia desde el flujo normal:
 *  solo existe compilada dentro del binario para que el exploit salte a ella.
 * ------------------------------------------------------------------------- */
void win(void) {
    char flag[128];
    FILE *f = fopen("./flag.txt", "r");
    if (f == NULL) {
        printf("\n[win] No se pudo abrir ./flag.txt\n");
        return;
    }
    printf("\n[win] Flag: ");
    while (fgets(flag, sizeof(flag), f) != NULL)
        fputs(flag, stdout);
    printf("\n");
    fclose(f);
}

/* -------------------------------------------------------------------------
 *  Busqueda en el "catalogo publico" (accesible SIN iniciar sesion).
 *
 *  >>> AQUI ESTA LA VULNERABILIDAD <<<
 *  'termino' mide 64 bytes, pero fgets puede escribir hasta 256. Un termino
 *  de mas de ~72 bytes desborda el buffer y sobrescribe la direccion de
 *  retorno guardada en la pila -> permite saltar a win() (ret2win).
 *
 *  La forma SEGURA seria:  fgets(termino, sizeof(termino), stdin);
 * ------------------------------------------------------------------------- */
void catalogo_publico(void) {
    char termino[64];

    titulo_seccion("Catalogo publico");
    msg_info("Busca un titulo en el catalogo publico de la biblioteca.");
    printf("  Termino de busqueda: ");
    fflush(stdout);

    /* vvv  linea vulnerable: 256 > sizeof(termino) == 64  vvv */
    if (fgets(termino, 256, stdin) == NULL) return;
    termino[strcspn(termino, "\n")] = '\0';

    printf("  No se han encontrado coincidencias para \"%s\".\n", termino);
}
