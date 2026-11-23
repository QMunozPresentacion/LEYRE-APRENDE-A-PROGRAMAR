# LEYRE-APRENDE-A-PROGRAMAR

leyre aprende a programar

## PRIMERA PARTE

make # compila con las flags vulnerables
echo "ssi{prueba}" > flag.txt

# Terminal 1 (el servicio):

socat TCP-LISTEN:4201,reuseaddr,fork EXEC:./biblioteca

# Terminal 2 (el ataque):

python3 exploit.py 127.0.0.1 4201

Cómo funciona la vulnerabilidad (de cero)

1. La idea de fondo: la pila y la "dirección de retorno"

Cuando tu programa entra en una función como catalogo_publico(), la CPU necesita recordar a dónde volver cuando esa función termine. Para eso, justo antes de saltar a la función, guarda en la pila la dirección de la siguiente instrucción. Eso es la dirección de retorno.

Dentro de la función, las variables locales (como char termino[64]) también viven en la pila, pegadas a esa dirección de retorno. El orden en memoria es este:

direcciones ALTAS
+----------------------------+
| direccion de retorno | <- a aqui vuelve la CPU al hacer 'ret'
+----------------------------+
| RBP guardado (8 bytes) |
+----------------------------+
| char termino[64] | <- tu buffer empieza aqui
+----------------------------+
direcciones BAJAS

El buffer se llena de abajo hacia arriba (hacia la dirección de retorno). Esa es la clave.

2. El fallo: leer más de lo que cabe

En catalogo.c la línea vulnerable es:

c
char termino[64];
fgets(termino, 256, stdin); // <-- MAL: dice 256, pero el buffer son 64

Le estás diciendo a fgets que puede escribir hasta 256 bytes, pero termino solo tiene 64. Si tú escribes un término de búsqueda de, digamos, 100 caracteres, fgets los copia todos, pasándose del buffer y machacando lo que hay encima: primero el RBP, y luego la dirección de retorno.

La versión correcta (segura) sería fgets(termino, sizeof(termino), stdin), que nunca escribiría más de 64. Esa es la única diferencia entre seguro y vulnerable.

3. El "offset": cuántos bytes hasta la dirección de retorno

No toda la basura que escribes cae en la dirección de retorno; primero hay que rellenar el buffer y el RBP. En tu binario, ese relleno son exactamente 72 bytes (los 64 del buffer + 8 del RBP + alineación). Lo medí con un patrón conocido en gdb:

[ 72 bytes cualquiera ][ los 8 bytes siguientes CAEN en la direccion de retorno ] 4. ret2win: ¿qué ponemos en la dirección de retorno?

Como queremos que el programa salte a win(), en esos 8 bytes ponemos la dirección de win(). El payload completo es:

payload = b"A"\*72 + direccion_de_win

Cuando catalogo_publico() termina y ejecuta ret, en vez de volver al menú, salta a win() → win() lee ./flag.txt y lo imprime. Eso es ret2win ("return to win").

Dos detalles de 64 bits:

La dirección se escribe en little-endian y ocupa 8 bytes (por eso p64() en el exploit).
A veces hay que poner antes un pequeño gadget ret para alinear la pila a 16 bytes; el exploit ya lo incluye por seguridad. 5. Por qué esas flags en el Makefile

El tema8 las llama "contramedidas". Para que el ataque sea posible hay que desactivarlas:

-fno-stack-protector → quita el canario, un valor centinela que detectaría el desbordamiento y abortaría.
-no-pie → hace que win() esté siempre en la misma dirección (si no, cambiaría en cada ejecución y no sabrías qué poner).
-z execstack → pila clásica. 6. Cómo lo mete el exploit por el menú

El servicio corre tras socat, así que el exploit se conecta por red, navega el menú (manda "3\n" para entrar en el catálogo público) y entonces envía el payload como si fuera el término de búsqueda:

conectar → "3\n" → ("A"\*72 + ret + win) → leer la flag
