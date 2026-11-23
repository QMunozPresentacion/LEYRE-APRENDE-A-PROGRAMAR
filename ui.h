/* ============================================================================
 *  ui.h  -  Interfaz de texto (TUI): colores, mensajes y entrada de datos
 * ========================================================================== */
#ifndef UI_H
#define UI_H

/* Codigos de color ANSI */
#define C_RESET   "\033[0m"
#define C_ROJO    "\033[1;31m"
#define C_VERDE   "\033[1;32m"
#define C_AMAR    "\033[1;33m"
#define C_AZUL    "\033[1;34m"
#define C_CYAN    "\033[1;36m"
#define C_BLANCO  "\033[1;37m"

/* --- Presentacion --- */
void limpiar_pantalla(void);
void pausar(void);
void banner(void);
void titulo_seccion(const char *texto);

/* --- Mensajes al usuario --- */
void msg_ok(const char *texto);
void msg_error(const char *texto);
void msg_info(const char *texto);

/* --- Entrada de datos --- */
/* Lee una linea completa, quita el '\n' final. Devuelve 1 si leyo algo. */
int  leer_linea(const char *prompt, char *destino, int tam);
/* Lee un entero validado. Devuelve -1 si la entrada no es un numero. */
int  leer_entero(const char *prompt);
/* Comprueba que una cadena no este vacia ni contenga los separadores ':' '|'. */
int  validar_campo(const char *s);

#endif /* UI_H */
