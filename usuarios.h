/* ============================================================================
 *  usuarios.h  -  Gestion de usuarios y sesion
 * ========================================================================== */
#ifndef USUARIOS_H
#define USUARIOS_H

/* Carga los usuarios desde el fichero (al arrancar el programa). */
void usuarios_cargar(void);

/* Operaciones interactivas (piden datos por pantalla). */
void usuarios_registrar(void);
int  usuarios_login(void);     /* devuelve 1 si el login es correcto */
void usuarios_logout(void);

/* Estado de la sesion actual. */
int         sesion_activa(void);
const char *sesion_usuario(void);

#endif /* USUARIOS_H */
