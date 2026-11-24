# Biblioteca personal

Programa de gestión de libros en C con interfaz TUI, gestión de usuarios,
persistencia en ficheros y **vulnerabilidad deliberada de Buffer Overflow**
para la primera parte de la práctica.

## Compilar y ejecutar

Necesitas GCC y Make. No se utilizan bibliotecas externas.

```bash
make
./biblioteca
```

Para recompilar desde cero:

```bash
make clean
make
```

## Vulnerabilidad (Primera Parte)

- Función vulnerable: `catalogo_publico()` (opción 3 del menú inicial)
- Buffer de 100 bytes (`texto[LEN_TITULO]`)
- `fgets` lee deliberadamente 612 bytes → overflow
- Offset hasta RIP: 0x78 (120 bytes)
- Función objetivo: `win()` (lee y muestra `./flag.txt`)

### Exploit

```bash
# Crear un flag de prueba
echo "flag{test}" > flag.txt

# Ejecutar el exploit (obtiene la dirección de win automáticamente)
python3 exploit.py
```

El exploit usa `nm` para localizar la dirección de `win` en el binario
compilado, por lo que no depende de una dirección fija.

## Organización

- main.c: inicio del programa, menús y sesión actual.
- ui.c / ui.h: ASCII, colores, mensajes y lectura de datos.
- usuarios.c / usuarios.h: registro y login consultando usuarios.txt.
- libros.c / libros.h: gestión de libros, catálogo público y las tres
  operaciones de procesamiento + función win.
- config.h: límites y nombres de los ficheros.
- exploit.py: exploit de la vulnerabilidad de Buffer Overflow.

Los usuarios se guardan en usuarios.txt con el formato usuario:contrasena.
Los libros se guardan en biblioteca.dat con el formato
usuario|titulo|autor|anio|genero|leido.
