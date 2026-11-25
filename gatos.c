#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "gatos.h"
#include "ui.h"

typedef struct {
    char owner[LEN_USER];
    char nombre[LEN_NOMBRE];
    char raza[LEN_RAZA];
    int edad;
    char color[LEN_COLOR];
    int adoptado;
} Gato;

static Gato gatos[MAX_GATOS];
static int cantidad = 0;

void gatos_imprimir_cabecera(void) {
    printf(C_BLANCO "  %-3s %-30s %-20s %-6s %-14s %-9s\n" C_RESET,
           "ID", "Nombre", "Raza", "Edad", "Color", "Estado");
    printf("  --------------------------------------------------------------------------------\n");
}

void gatos_imprimir_fila(int id, const Gato *G) {
    printf("  %-3d %-30.30s %-20.20s %-6d %-14.14s %-9s\n",
           id, G->nombre, G->raza, G->edad, G->color,
           G->adoptado ? "Adoptado" : "Pendiente");
}

static int guardar(void) {
    FILE *f = fopen(FICHERO_GATOS ".tmp", "w");
    if (f == NULL) {
        msg_error("No se pudo abrir el fichero de gatos.");
        return 0;
    }
    int error = 0;
    for (int i = 0; i < cantidad; i++) {
        Gato *g = &gatos[i];
        if (fprintf(f, "%s|%s|%s|%d|%s|%d\n", g->owner, g->nombre,
                    g->raza, g->edad, g->color, g->adoptado) < 0) error = 1;
    }
    if (fclose(f) != 0) error = 1;
    if (!error && rename(FICHERO_GATOS ".tmp", FICHERO_GATOS) != 0)
        error = 1;
    if (error) {
        remove(FICHERO_GATOS ".tmp");
        msg_error("No se pudieron guardar los gatos.");
    }
    return !error;
}

static int gato_valido(const Gato *g) {
    return validar_campo(g->owner) && validar_campo(g->nombre) &&
           validar_campo(g->raza) && validar_campo(g->color) &&
           g->edad >= 0 && g->edad <= 2100 && (g->adoptado == 0 || g->adoptado == 1);
}

/* Separar los campos evita depender de anchuras fijas en sscanf. */
static int parsear_gato(char *linea, Gato *g) {
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
    if (strlen(campos[0]) >= sizeof(g->owner) ||
        strlen(campos[1]) >= sizeof(g->nombre) ||
        strlen(campos[2]) >= sizeof(g->raza) ||
        strlen(campos[4]) >= sizeof(g->color)) return 0;

    char *fin;
    errno = 0;
    long edad = strtol(campos[3], &fin, 10);
    if (errno || fin == campos[3] || *fin || edad < 0 || edad > 2100)
        return 0;
    if (strcmp(campos[5], "0") != 0 && strcmp(campos[5], "1") != 0)
        return 0;

    strcpy(g->owner, campos[0]);
    strcpy(g->nombre, campos[1]);
    strcpy(g->raza, campos[2]);
    strcpy(g->color, campos[4]);
    g->edad = (int)edad;
    g->adoptado = campos[5][0] - '0';
    return gato_valido(g);
}

void gatos_cargar(void) {
    FILE *f = fopen(FICHERO_GATOS, "r");
    if (f == NULL) {
        if (errno == ENOENT) cantidad = 0;
        else msg_error("No se pudieron leer los gatos.");
        return;
    }
    cantidad = 0;
    char linea[LEN_LINEA];
    while (fgets(linea, sizeof(linea), f)) {
        if (strchr(linea, '\n') == NULL) {
            int c = fgetc(f);
            if (c != EOF && c != '\n') {
                while ((c = fgetc(f)) != EOF && c != '\n') {}
                msg_error("Registro de gato demasiado largo: se ha omitido.");
                continue;
            }
        }
        linea[strcspn(linea, "\r\n")] = '\0';
        Gato g;
        if (!parsear_gato(linea, &g)) {
            msg_error("Registro de gato incorrecto: se ha omitido.");
        } else if (cantidad < MAX_GATOS) {
            gatos[cantidad++] = g;
        } else {
            msg_error("No hay mas sitio para gatos: se han omitido registros.");
            break;
        }
    }
    if (ferror(f)) msg_error("Error al leer los gatos.");
    fclose(f);
}

/* Se usa tanto al crear como al modificar un gato. */
static int pedir_gato(Gato *g) {
    leer_linea("  Nombre: ", g->nombre, sizeof(g->nombre));
    leer_linea("  Raza:   ", g->raza, sizeof(g->raza));
    g->edad = leer_entero("  Edad del gato: ");
    leer_linea("  Color: ", g->color, sizeof(g->color));
    g->adoptado = leer_entero("  Adoptado? (1 = si, 0 = no): ");
    if (!gato_valido(g)) {
        msg_error("Datos no validos.");
        return 0;
    }
    return 1;
}

void gatos_agregar(const char *owner) {
    titulo_seccion("Anadir un gato");
    if (cantidad >= MAX_GATOS) {
        msg_error("No hay mas sitio para gatos.");
        return;
    }
    Gato nuevo;
    if (owner == NULL || strlen(owner) >= sizeof(nuevo.owner) ||
        !validar_campo(owner)) {
        msg_error("Propietario no valido.");
        return;
    }
    strcpy(nuevo.owner, owner);
    if (!pedir_gato(&nuevo)) return;
    gatos[cantidad++] = nuevo;
    if (guardar()) msg_ok("Gato anadido a tu refugio.");
    else cantidad--;
}

void gatos_listar(const char *owner) {
    titulo_seccion("Mis gatos");
    gatos_imprimir_cabecera();
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(gatos[i].owner, owner) == 0)
            gatos_imprimir_fila(i + 1, &gatos[i]);
    }
}

/* El ID es la posicion en el array + 1. Se comprueba el propietario. */
static int elegir_gato(const char *owner) {
    gatos_listar(owner);
    int id = leer_entero("  ID del gato (0 = cancelar): ");
    if (id <= 0 || id > cantidad || strcmp(gatos[id - 1].owner, owner) != 0) {
        msg_info("Seleccion no valida u operacion cancelada.");
        return -1;
    }
    return id - 1;
}

void gatos_modificar(const char *owner) {
    int i = elegir_gato(owner);
    if (i == -1) return;
    Gato nuevo = gatos[i];
    msg_info("Introduce de nuevo todos los datos del gato.");
    if (!pedir_gato(&nuevo)) return;
    Gato anterior = gatos[i];
    gatos[i] = nuevo;
    if (guardar()) msg_ok("Gato modificado.");
    else gatos[i] = anterior;
}

void gatos_eliminar(const char *owner) {
    int i = elegir_gato(owner);
    if (i == -1) return;
    Gato anterior = gatos[i];
    for (int j = i; j < cantidad - 1; j++) gatos[j] = gatos[j + 1];
    cantidad--;
    if (guardar()) msg_ok("Gato eliminado.");
    else {
        for (int j = cantidad; j > i; j--) gatos[j] = gatos[j - 1];
        gatos[i] = anterior;
        cantidad++;
    }
}

void gatos_buscar(const char *owner) {
    char texto[LEN_NOMBRE];
    titulo_seccion("Buscar en mis gatos");
    printf("  Criterios:  1) Nombre   2) Raza   3) Color\n");
    int criterio = leer_entero("  Elige criterio: ");
    if (criterio < 1 || criterio > 3) {
        msg_error("Criterio no valido.");
        return;
    }
    leer_linea("  Texto a buscar: ", texto, sizeof(texto));
    gatos_imprimir_cabecera();
    for (int i = 0; i < cantidad; i++) {
        Gato *g = &gatos[i];
        const char *campo = g->nombre;
        if (criterio == 2) campo = g->raza;
        if (criterio == 3) campo = g->color;
        if (strcmp(g->owner, owner) == 0 && strstr(campo, texto))
            gatos_imprimir_fila(i + 1, g);
    }
}

void catalogo_publico(void) {
    char texto[LEN_NOMBRE];

    titulo_seccion("Catalogo publico");
    msg_info("Busca un nombre en el catalogo publico del refugio.");

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

    gatos_imprimir_cabecera();

    for (int i = 0; i < cantidad; i++) {
        if (strstr(gatos[i].nombre, texto))
            gatos_imprimir_fila(i + 1, &gatos[i]);
    }
}

void proc_ordenar_por_edad(const char *owner) {
    int lista[MAX_GATOS];
    int n = 0;
    titulo_seccion("Gatos ordenados por edad");
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(gatos[i].owner, owner) == 0) lista[n++] = i;
    }
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (gatos[lista[i]].edad > gatos[lista[j]].edad) {
                int aux = lista[i];
                lista[i] = lista[j];
                lista[j] = aux;
            }
        }
    }
    gatos_imprimir_cabecera();
    for (int i = 0; i < n; i++) gatos_imprimir_fila(lista[i] + 1, &gatos[lista[i]]);
}

void proc_filtrar_por_estado(const char *owner) {
    titulo_seccion("Filtrar por estado");
    int estado = leer_entero("  1) Adoptados   0) Pendientes : ");
    if (estado != 0 && estado != 1) {
        msg_error("Opcion no valida.");
        return;
    }
    gatos_imprimir_cabecera();
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(gatos[i].owner, owner) == 0 && gatos[i].adoptado == estado)
            gatos_imprimir_fila(i + 1, &gatos[i]);
    }
}

void proc_estadisticas(const char *owner) {
    int total = 0, adoptados = 0;
    titulo_seccion("Estadisticas de mi refugio");
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(gatos[i].owner, owner) == 0) {
            total++;
            adoptados += gatos[i].adoptado;
        }
    }
    printf("  Total de gatos ........... %d\n", total);
    printf("  Adoptados ................ %d\n", adoptados);
    printf("  Pendientes ............... %d\n", total - adoptados);
    if (total > 0) printf("  Porcentaje adoptado ...... %.1f%%\n", 100.0 * adoptados / total);
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
