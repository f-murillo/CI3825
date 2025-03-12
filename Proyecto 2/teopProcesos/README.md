# Solución al problema del Teatro de Operaciones usando Procesos

### Creación del ejecutable

```
make 
```

### Creación del ejecutable detectando errores en el código antes de su ejecución.

```
scan-build make
```

### Ejecución del programa

```
./teopp <numero de procesos> <instancia>
```

> [!NOTE]
> El programa asume que el número de procesos ingresado es un entero

### Ejecución del programa con Valgrind para comprobar gestión de la memoria y posibles errores

```
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  ./teopp <numero de procesos> <instancia>
```

### En caso de querer compilarlo y ejecutarlo en Windows con gcc (cmd o PowerShell)

```
gcc main.c teatro.c -o teopp
```
```
teopp <numero de procesos> <instancia>
```

