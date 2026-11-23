/* ============================================================================
 *  main.c  -  Gestor de Biblioteca Personal (SSI, Primera Parte)
 *
 *  Punto de entrada. Carga los datos de los ficheros y lanza la interfaz.
 *  La aplicacion es completamente funcional y SIN vulnerabilidad.
 * ========================================================================== */
#include <stdio.h>
#include "ui.h"
#include "usuarios.h"
#include "libros.h"
#include "menus.h"

int main(void) {
    /* Salida sin buffer: util cuando el programa corra tras socat. */
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin,  NULL, _IONBF, 0);

    /* Persistencia: recuperar la informacion guardada en ejecuciones previas. */
    usuarios_cargar();
    libros_cargar();

    menu_inicial();

    limpiar_pantalla();
    msg_info("Hasta pronto.");
    return 0;
}
