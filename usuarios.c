#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "usuarios.h"
#include "ui.h"

/* Busca en el fichero: 1 = encontrado, 0 = no existe, -1 = error. */
static int buscar_usuario(const char *nombre, char *password) {
    FILE *f = fopen(FICHERO_USUARIOS, "r");
    if (f == NULL) {
        if (errno == ENOENT) return 0;
        msg_error("No se pudieron leer los usuarios.");
        return -1;
    }
    char linea[LEN_LINEA], usuario[LEN_USER], clave[LEN_PASS];
    int encontrado = 0;
    while (fgets(linea, sizeof(linea), f)) {
        if (sscanf(linea, "%49[^:]:%49[^\r\n]", usuario, clave) != 2) continue;
        if (strcmp(usuario, nombre) == 0) {
            strcpy(password, clave);
            encontrado = 1;
            break;
        }
    }
    if (ferror(f)) encontrado = -1;
    fclose(f);
    if (encontrado == -1) msg_error("Error al leer los usuarios.");
    return encontrado;
}

void usuarios_registrar(void) {
    char nombre[LEN_USER], password[LEN_PASS], guardada[LEN_PASS];
    titulo_seccion("Registro de nuevo usuario");
    leer_linea("  Nombre de usuario: ", nombre, sizeof(nombre));
    leer_linea("  Contrasena: ", password, sizeof(password));
    if (!validar_campo(nombre) || !validar_campo(password)) {
        msg_error("Datos vacios o con separadores ':' '|'.");
        return;
    }
    int resultado = buscar_usuario(nombre, guardada);
    if (resultado == -1) return;
    if (resultado == 1) {
        msg_error("Ese usuario ya existe.");
        return;
    }
    FILE *f = fopen(FICHERO_USUARIOS, "a");
    if (f == NULL) {
        msg_error("No se pudo abrir el fichero de usuarios.");
        return;
    }
    int error = fprintf(f, "%s:%s\n", nombre, password) < 0;
    if (fclose(f) != 0) error = 1;
    if (error) msg_error("No se pudo guardar el usuario.");
    else msg_ok("Usuario registrado correctamente.");
}

int usuarios_login(char *usuario) {
    char password[LEN_PASS], guardada[LEN_PASS];
    titulo_seccion("Inicio de sesion");
    leer_linea("  Usuario: ", usuario, LEN_USER);
    leer_linea("  Contrasena: ", password, sizeof(password));
    int resultado = buscar_usuario(usuario, guardada);
    if (resultado == -1) return 0;
    if (resultado == 0 || strcmp(guardada, password) != 0) {
        msg_error("Usuario o contrasena incorrectos.");
        return 0;
    }
    msg_ok("Sesion iniciada.");
    return 1;
}
