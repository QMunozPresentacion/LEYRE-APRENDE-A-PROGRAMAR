#include <stdio.h>
#include "ui.h"
#include "usuarios.h"
#include "libros.h"


static void menu_libros(const char *owner) {
    int opcion;
    do {
        limpiar_pantalla();
        titulo_seccion("Gestion de libros");
        printf("  1) Anadir libro\n"
               "  2) Listar mis libros\n"
               "  3) Buscar en mis libros\n"
               "  4) Modificar un libro\n"
               "  5) Eliminar un libro\n"
               "  0) Volver\n");
        opcion = leer_entero("\n  Opcion: ");

        switch (opcion) {
            case 1: libros_agregar(owner); break;
            case 2: libros_listar(owner);  break;
            case 3: libros_buscar(owner);  break;
            case 4: libros_modificar(owner); break;
            case 5: libros_eliminar(owner);  break;
            case 0: break;
            default: msg_error("Opcion no valida."); break;
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

static void menu_procesamiento(const char *owner) {
    int opcion;
    do {
        limpiar_pantalla();
        titulo_seccion("Procesamiento de informacion");
        printf("  1) Ordenar libros por anio\n"
               "  2) Filtrar por estado (leido/pendiente)\n"
               "  3) Estadisticas y resumen\n"
               "  0) Volver\n");
        opcion = leer_entero("\n  Opcion: ");

        switch (opcion) {
            case 1: proc_ordenar_por_anio(owner); break;
            case 2: proc_filtrar_por_estado(owner); break;
            case 3: proc_estadisticas(owner);     break;
            case 0: break;
            default: msg_error("Opcion no valida."); break;
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

static int menu_principal(const char *usuario) {
    int opcion;
    do {
        limpiar_pantalla();
        printf(C_VERDE "\n  Sesion: %s\n" C_RESET, usuario);
        titulo_seccion("Menu principal");
        printf("  1) Gestion de libros\n"
               "  2) Procesamiento de informacion\n"
               "  3) Cerrar sesion\n"
               "  0) Salir del programa\n");
        opcion = leer_entero("\n  Opcion: ");

        switch (opcion) {
            case 1: menu_libros(usuario);        break;
            case 2: menu_procesamiento(usuario); break;
            case 3: msg_info("Sesion cerrada."); pausar(); return 1;
            case 0: return 0;
            default: msg_error("Opcion no valida."); pausar(); break;
        }
    } while (1);
}

static void menu_inicial(void) {
    int opcion;
    char usuario[LEN_USER];
    do {
        limpiar_pantalla();
        banner();
        titulo_seccion("Bienvenido/a");
        printf("  1) Iniciar sesion\n"
               "  2) Registrarse\n"
               "  3) Buscar en el catalogo publico\n"
               "  0) Salir\n");
        opcion = leer_entero("\n  Opcion: ");

        switch (opcion) {
            case 1:
                if (usuarios_login(usuario)) {
                    pausar();
                    if (!menu_principal(usuario)) return;
                }
                else pausar();
                break;
            case 2:
                usuarios_registrar(); pausar(); break;
            case 3:
                catalogo_publico(); pausar(); break;
            case 0:
                break;
            default:
                msg_error("Opcion no valida."); pausar(); break;
        }
    } while (opcion != 0);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);
    libros_cargar();
    menu_inicial();
    msg_info("Hasta pronto.");
    return 0;
}
