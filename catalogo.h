/* ============================================================================
 *  catalogo.h  -  Catalogo publico (modulo que CONTIENE la vulnerabilidad)
 * ----------------------------------------------------------------------------
 *  Este modulo aisla deliberadamente el Buffer Overflow del trabajo:
 *    - catalogo_publico(): lee un termino de busqueda en un buffer de la pila
 *      usando un tamano mayor que el del buffer  ->  desbordamiento.
 *    - win(): funcion objetivo del exploit; lee ./flag.txt. No se llama desde
 *      ningun punto del flujo normal del programa.
 * ========================================================================== */
#ifndef CATALOGO_H
#define CATALOGO_H

/* Funcion objetivo del exploit (ret2win). No se invoca nunca directamente. */
void win(void);

/* Busqueda en el catalogo publico (accesible sin iniciar sesion). VULNERABLE. */
void catalogo_publico(void);

#endif /* CATALOGO_H */
