Creación del ejecutable

make 

Creación del ejecutable detectando errores en el código antes de su ejecución.

scan-build make

Ejecución del programa

./teopp <numero de procesos> <instancia> (el programa asume que el numero de procesos ingresado es un entero)

Ejecución del programa con Valgrind para comprobar gestión de la memoria y posibles errores

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  ./teopp <numero de procesos> <instancia>
