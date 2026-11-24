#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "libros.h"
#include "ui.h"

typedef struct {
    char owner[LEN_USER];
    char titulo[LEN_TITULO];
    char autor[LEN_AUTOR];
    int anio;
    char genero[LEN_GENERO];
    int leido;
} Libro;

static Libro biblioteca[MAX_LIBROS];
static int cantidad = 0;

void libros_imprimir_cabecera(void) {
    printf(C_BLANCO "  %-3s %-30s %-20s %-6s %-14s %-9s\n" C_RESET,
           "ID", "Titulo", "Autor", "Anio", "Genero", "Estado");
    printf("  --------------------------------------------------------------------------------\n");
}

void libros_imprimir_fila(int id, const Libro *L) {
    printf("  %-3d %-30.30s %-20.20s %-6d %-14.14s %-9s\n",
           id, L->titulo, L->autor, L->anio, L->genero,
           L->leido ? "Leido" : "Pendiente");
}

static int guardar(void) {
    FILE *f = fopen(FICHERO_LIBROS ".tmp", "w");
    if (f == NULL) {
        msg_error("No se pudo abrir el fichero de libros.");
        return 0;
    }
    int error = 0;
    for (int i = 0; i < cantidad; i++) {
        Libro *l = &biblioteca[i];
        if (fprintf(f, "%s|%s|%s|%d|%s|%d\n", l->owner, l->titulo,
                    l->autor, l->anio, l->genero, l->leido) < 0) error = 1;
    }
    if (fclose(f) != 0) error = 1;
    if (!error && rename(FICHERO_LIBROS ".tmp", FICHERO_LIBROS) != 0)
        error = 1;
    if (error) {
        remove(FICHERO_LIBROS ".tmp");
        msg_error("No se pudieron guardar los libros.");
    }
    return !error;
}

static int libro_valido(const Libro *l) {
    return validar_campo(l->owner) && validar_campo(l->titulo) &&
           validar_campo(l->autor) && validar_campo(l->genero) &&
           l->anio >= 0 && l->anio <= 2100 && (l->leido == 0 || l->leido == 1);
}

/* Separar los campos evita depender de anchuras fijas en sscanf. */
static int parsear_libro(char *linea, Libro *l) {
    char *campos[6];
    char *p = linea;
    for (int i = 0; i < 6; i++) {
        campos[i] = p;
        char *separador = strchr(p, '|');
        if (i == 5) {
            if (separador != NULL) return 0;
        } else {
            if (separador == NULL) return 0;
            *separador = '\0';
            p = separador + 1;
        }
    }
    if (strlen(campos[0]) >= sizeof(l->owner) ||
        strlen(campos[1]) >= sizeof(l->titulo) ||
        strlen(campos[2]) >= sizeof(l->autor) ||
        strlen(campos[4]) >= sizeof(l->genero)) return 0;

    char *fin;
    errno = 0;
    long anio = strtol(campos[3], &fin, 10);
    if (errno || fin == campos[3] || *fin || anio < 0 || anio > 2100)
        return 0;
    if (strcmp(campos[5], "0") != 0 && strcmp(campos[5], "1") != 0)
        return 0;

    strcpy(l->owner, campos[0]);
    strcpy(l->titulo, campos[1]);
    strcpy(l->autor, campos[2]);
    strcpy(l->genero, campos[4]);
    l->anio = (int)anio;
    l->leido = campos[5][0] - '0';
    return libro_valido(l);
}

void libros_cargar(void) {
    FILE *f = fopen(FICHERO_LIBROS, "r");
    if (f == NULL) {
        if (errno == ENOENT) cantidad = 0;
        else msg_error("No se pudieron leer los libros.");
        return;
    }
    cantidad = 0;
    char linea[LEN_LINEA];
    while (fgets(linea, sizeof(linea), f)) {
        if (strchr(linea, '\n') == NULL) {
            int c = fgetc(f);
            if (c != EOF && c != '\n') {
                while ((c = fgetc(f)) != EOF && c != '\n') {}
                msg_error("Registro de libro demasiado largo: se ha omitido.");
                continue;
            }
        }
        linea[strcspn(linea, "\r\n")] = '\0';
        Libro l;
        if (!parsear_libro(linea, &l)) {
            msg_error("Registro de libro incorrecto: se ha omitido.");
        } else if (cantidad < MAX_LIBROS) {
            biblioteca[cantidad++] = l;
        } else {
            msg_error("La biblioteca esta llena: se han omitido registros.");
            break;
        }
    }
    if (ferror(f)) msg_error("Error al leer los libros.");
    fclose(f);
}

/* Se usa tanto al crear como al modificar un libro. */
static int pedir_libro(Libro *l) {
    leer_linea("  Titulo: ", l->titulo, sizeof(l->titulo));
    leer_linea("  Autor:  ", l->autor, sizeof(l->autor));
    l->anio = leer_entero("  Anio de publicacion: ");
    leer_linea("  Genero: ", l->genero, sizeof(l->genero));
    l->leido = leer_entero("  Leido? (1 = si, 0 = no): ");
    if (!libro_valido(l)) {
        msg_error("Datos no validos.");
        return 0;
    }
    return 1;
}

void libros_agregar(const char *owner) {
    titulo_seccion("Anadir un libro");
    if (cantidad >= MAX_LIBROS) {
        msg_error("La biblioteca esta llena.");
        return;
    }
    Libro nuevo;
    if (owner == NULL || strlen(owner) >= sizeof(nuevo.owner) ||
        !validar_campo(owner)) {
        msg_error("Propietario no valido.");
        return;
    }
    strcpy(nuevo.owner, owner);
    if (!pedir_libro(&nuevo)) return;
    biblioteca[cantidad++] = nuevo;
    if (guardar()) msg_ok("Libro anadido a tu biblioteca.");
    else cantidad--;
}

void libros_listar(const char *owner) {
    titulo_seccion("Mis libros");
    libros_imprimir_cabecera();
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(biblioteca[i].owner, owner) == 0)
            libros_imprimir_fila(i + 1, &biblioteca[i]);
    }
}

/* El ID es la posicion en el array + 1. Se comprueba el propietario. */
static int elegir_libro(const char *owner) {
    libros_listar(owner);
    int id = leer_entero("  ID del libro (0 = cancelar): ");
    if (id <= 0 || id > cantidad || strcmp(biblioteca[id - 1].owner, owner) != 0) {
        msg_info("Seleccion no valida u operacion cancelada.");
        return -1;
    }
    return id - 1;
}

void libros_modificar(const char *owner) {
    int i = elegir_libro(owner);
    if (i == -1) return;
    Libro nuevo = biblioteca[i];
    msg_info("Introduce de nuevo todos los datos del libro.");
    if (!pedir_libro(&nuevo)) return;
    Libro anterior = biblioteca[i];
    biblioteca[i] = nuevo;
    if (guardar()) msg_ok("Libro modificado.");
    else biblioteca[i] = anterior;
}

void libros_eliminar(const char *owner) {
    int i = elegir_libro(owner);
    if (i == -1) return;
    Libro anterior = biblioteca[i];
    for (int j = i; j < cantidad - 1; j++) biblioteca[j] = biblioteca[j + 1];
    cantidad--;
    if (guardar()) msg_ok("Libro eliminado.");
    else {
        for (int j = cantidad; j > i; j--) biblioteca[j] = biblioteca[j - 1];
        biblioteca[i] = anterior;
        cantidad++;
    }
}

void libros_buscar(const char *owner) {
    char texto[LEN_TITULO];
    titulo_seccion("Buscar en mis libros");
    printf("  Criterios:  1) Titulo   2) Autor   3) Genero\n");
    int criterio = leer_entero("  Elige criterio: ");
    if (criterio < 1 || criterio > 3) {
        msg_error("Criterio no valido.");
        return;
    }
    leer_linea("  Texto a buscar: ", texto, sizeof(texto));
    libros_imprimir_cabecera();
    for (int i = 0; i < cantidad; i++) {
        Libro *l = &biblioteca[i];
        const char *campo = l->titulo;
        if (criterio == 2) campo = l->autor;
        if (criterio == 3) campo = l->genero;
        if (strcmp(l->owner, owner) == 0 && strstr(campo, texto))
            libros_imprimir_fila(i + 1, l);
    }
}

void catalogo_publico(void) {
    char texto[LEN_TITULO];

    titulo_seccion("Catalogo publico");
    msg_info("Busca un titulo en el catalogo publico de la biblioteca.");

    printf("  Termino de busqueda: ");
    fflush(stdout);

    /*VULNERABILIDAD*/
    if (fgets(texto, sizeof(texto) + 512, stdin) == NULL) {
        msg_error("No se pudo leer la busqueda.");
        return;
    }

    texto[strcspn(texto, "\n")] = '\0';

    if (!validar_campo(texto)) {
        msg_error("Termino de busqueda no valido.");
        return;
    }

    libros_imprimir_cabecera();

    for (int i = 0; i < cantidad; i++) {
        if (strstr(biblioteca[i].titulo, texto))
            libros_imprimir_fila(i + 1, &biblioteca[i]);
    }
}

void proc_ordenar_por_anio(const char *owner) {
    int lista[MAX_LIBROS];
    int n = 0;
    titulo_seccion("Libros ordenados por anio");
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(biblioteca[i].owner, owner) == 0) lista[n++] = i;
    }
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (biblioteca[lista[i]].anio > biblioteca[lista[j]].anio) {
                int aux = lista[i];
                lista[i] = lista[j];
                lista[j] = aux;
            }
        }
    }
    libros_imprimir_cabecera();
    for (int i = 0; i < n; i++) libros_imprimir_fila(lista[i] + 1, &biblioteca[lista[i]]);
}

void proc_filtrar_por_estado(const char *owner) {
    titulo_seccion("Filtrar por estado");
    int estado = leer_entero("  1) Leidos   0) Pendientes : ");
    if (estado != 0 && estado != 1) {
        msg_error("Opcion no valida.");
        return;
    }
    libros_imprimir_cabecera();
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(biblioteca[i].owner, owner) == 0 && biblioteca[i].leido == estado)
            libros_imprimir_fila(i + 1, &biblioteca[i]);
    }
}

void proc_estadisticas(const char *owner) {
    int total = 0, leidos = 0;
    titulo_seccion("Estadisticas de mi biblioteca");
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(biblioteca[i].owner, owner) == 0) {
            total++;
            leidos += biblioteca[i].leido;
        }
    }
    printf("  Total de libros .......... %d\n", total);
    printf("  Leidos ................... %d\n", leidos);
    printf("  Pendientes ............... %d\n", total - leidos);
    if (total > 0) printf("  Porcentaje leido ......... %.1f%%\n", 100.0 * leidos / total);
}

void win(void) {
    FILE *f = fopen("./flag.txt", "r");

    if (f == NULL) {
        perror("No se pudo abrir ./flag.txt");
        return;
    }

    int c;
    while ((c = fgetc(f)) != EOF) {
        putchar(c);
    }

    fclose(f);
    fflush(stdout);
}
