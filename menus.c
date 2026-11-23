/* ============================================================================
 *  menus.c  -  Menus y navegacion. Conecta la interfaz con los gestores.
 * ========================================================================== */
#include <stdio.h>
#include "ui.h"
#include "usuarios.h"
#include "libros.h"
#include "procesamiento.h"
#include "menus.h"

/* --------------------------- Submenu: libros ----------------------------- */
static void menu_libros(void) {
    const char *owner = sesion_usuario();
    int opcion;
    do {
        limpiar_pantalla();
        titulo_seccion("Gestion de libros");
        printf("  1) Anadir libro\n");
        printf("  2) Listar mis libros\n");
        printf("  3) Buscar en mis libros\n");
        printf("  4) Modificar un libro\n");
        printf("  5) Eliminar un libro\n");
        printf("  0) Volver\n");
        opcion = leer_entero("\n  Opcion: ");

        switch (opcion) {
            case 1: libros_agregar(owner);   pausar(); break;
            case 2: libros_listar(owner);    pausar(); break;
            case 3: libros_buscar(owner);    pausar(); break;
            case 4: libros_modificar(owner); pausar(); break;
            case 5: libros_eliminar(owner);  pausar(); break;
            case 0: break;
            default: msg_error("Opcion no valida."); pausar(); break;
        }
    } while (opcion != 0);
}

/* ----------------------- Submenu: procesamiento -------------------------- */
static void menu_procesamiento(void) {
    const char *owner = sesion_usuario();
    int opcion;
    do {
        limpiar_pantalla();
        titulo_seccion("Procesamiento de informacion");
        printf("  1) Ordenar libros por anio\n");
        printf("  2) Filtrar por estado (leido/pendiente)\n");
        printf("  3) Estadisticas y resumen\n");
        printf("  0) Volver\n");
        opcion = leer_entero("\n  Opcion: ");

        switch (opcion) {
            case 1: proc_ordenar_por_anio(owner);   pausar(); break;
            case 2: proc_filtrar_por_estado(owner); pausar(); break;
            case 3: proc_estadisticas(owner);       pausar(); break;
            case 0: break;
            default: msg_error("Opcion no valida."); pausar(); break;
        }
    } while (opcion != 0);
}

/* --------------------- Menu principal (con sesion) ----------------------- */
static void menu_principal(void) {
    int opcion;
    do {
        limpiar_pantalla();
        printf(C_VERDE "\n  Sesion: %s\n" C_RESET, sesion_usuario());
        titulo_seccion("Menu principal");
        printf("  1) Gestion de libros\n");
        printf("  2) Procesamiento de informacion\n");
        printf("  3) Cerrar sesion\n");
        printf("  0) Salir del programa\n");
        opcion = leer_entero("\n  Opcion: ");

        switch (opcion) {
            case 1: menu_libros();        break;
            case 2: menu_procesamiento(); break;
            case 3: usuarios_logout(); pausar(); return;
            case 0: return;    /* la salida la gestiona el menu inicial */
            default: msg_error("Opcion no valida."); pausar(); break;
        }
    } while (1);
}

/* -------------------------- Menu inicial --------------------------------- */
void menu_inicial(void) {
    int opcion;
    do {
        limpiar_pantalla();
        banner();
        titulo_seccion("Bienvenido/a");
        printf("  1) Iniciar sesion\n");
        printf("  2) Registrarse\n");
        printf("  0) Salir\n");
        opcion = leer_entero("\n  Opcion: ");

        switch (opcion) {
            case 1:
                if (usuarios_login()) { pausar(); menu_principal(); }
                else pausar();
                break;
            case 2:
                usuarios_registrar(); pausar(); break;
            case 0:
                break;
            default:
                msg_error("Opcion no valida."); pausar(); break;
        }
    } while (opcion != 0);
}
