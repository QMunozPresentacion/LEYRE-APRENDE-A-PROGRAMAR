/* ============================================================================
 *  usuarios.c  -  Implementacion de la gestion de usuarios
 *
 *  Formato del fichero usuarios.txt (una linea por usuario):
 *      usuario:contrasena
 * ========================================================================== */
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "ui.h"
#include "usuarios.h"

/* Un usuario del sistema */
typedef struct {
    char usuario[LEN_USER];
    char password[LEN_PASS];
} Usuario;

/* Estado interno del modulo (privado: 'static') */
static Usuario usuarios[MAX_USUARIOS];
static int     num_usuarios = 0;
static char    usuario_actual[LEN_USER] = "";

/* ------------------------- Funciones auxiliares -------------------------- */

/* Devuelve el indice del usuario con ese nombre, o -1 si no existe. */
static int indice_usuario(const char *nombre) {
    for (int i = 0; i < num_usuarios; i++)
        if (strcmp(usuarios[i].usuario, nombre) == 0)
            return i;
    return -1;
}

/* Vuelca todos los usuarios al fichero. Devuelve 1 si tuvo exito. */
static int guardar_usuarios(void) {
    FILE *f = fopen(FICHERO_USUARIOS, "w");
    if (f == NULL) {
        msg_error("No se pudo escribir el fichero de usuarios.");
        return 0;
    }
    for (int i = 0; i < num_usuarios; i++)
        fprintf(f, "%s:%s\n", usuarios[i].usuario, usuarios[i].password);
    fclose(f);
    return 1;
}

/* ----------------------------- Interfaz publica -------------------------- */

void usuarios_cargar(void) {
    num_usuarios = 0;
    FILE *f = fopen(FICHERO_USUARIOS, "r");
    if (f == NULL) return;              /* primera ejecucion: aun no existe */

    char linea[LEN_LINEA];
    while (fgets(linea, sizeof(linea), f) && num_usuarios < MAX_USUARIOS) {
        linea[strcspn(linea, "\n")] = '\0';
        char *sep = strchr(linea, ':');
        if (sep == NULL) continue;      /* linea mal formada: se ignora */
        *sep = '\0';

        strncpy(usuarios[num_usuarios].usuario,  linea,   LEN_USER - 1);
        strncpy(usuarios[num_usuarios].password, sep + 1, LEN_PASS - 1);
        usuarios[num_usuarios].usuario[LEN_USER - 1]  = '\0';
        usuarios[num_usuarios].password[LEN_PASS - 1] = '\0';
        num_usuarios++;
    }
    fclose(f);
}

void usuarios_registrar(void) {
    titulo_seccion("Registro de nuevo usuario");

    if (num_usuarios >= MAX_USUARIOS) {
        msg_error("Se ha alcanzado el limite de usuarios.");
        return;
    }

    char nombre[LEN_USER], pass[LEN_PASS], pass2[LEN_PASS];
    leer_linea("  Nombre de usuario: ", nombre, sizeof(nombre));

    if (!validar_campo(nombre)) {
        msg_error("Usuario no valido (vacio o contiene ':' '|').");
        return;
    }
    if (indice_usuario(nombre) != -1) {     /* control de duplicados */
        msg_error("Ese usuario ya existe.");
        return;
    }

    leer_linea("  Contrasena: ", pass, sizeof(pass));
    leer_linea("  Repite la contrasena: ", pass2, sizeof(pass2));

    if (!validar_campo(pass)) {
        msg_error("Contrasena no valida.");
        return;
    }
    if (strcmp(pass, pass2) != 0) {
        msg_error("Las contrasenas no coinciden.");
        return;
    }

    strncpy(usuarios[num_usuarios].usuario,  nombre, LEN_USER - 1);
    strncpy(usuarios[num_usuarios].password, pass,   LEN_PASS - 1);
    usuarios[num_usuarios].usuario[LEN_USER - 1]  = '\0';
    usuarios[num_usuarios].password[LEN_PASS - 1] = '\0';
    num_usuarios++;

    if (guardar_usuarios())
        msg_ok("Usuario registrado correctamente.");
}

int usuarios_login(void) {
    titulo_seccion("Inicio de sesion");

    char nombre[LEN_USER], pass[LEN_PASS];
    leer_linea("  Usuario: ", nombre, sizeof(nombre));
    leer_linea("  Contrasena: ", pass, sizeof(pass));

    int idx = indice_usuario(nombre);
    if (idx == -1 || strcmp(usuarios[idx].password, pass) != 0) {
        msg_error("Usuario o contrasena incorrectos.");
        return 0;
    }

    strncpy(usuario_actual, nombre, LEN_USER - 1);
    usuario_actual[LEN_USER - 1] = '\0';
    msg_ok("Sesion iniciada.");
    return 1;
}

void usuarios_logout(void) {
    usuario_actual[0] = '\0';
    msg_info("Sesion cerrada.");
}

int sesion_activa(void) {
    return usuario_actual[0] != '\0';
}

const char *sesion_usuario(void) {
    return usuario_actual;
}
