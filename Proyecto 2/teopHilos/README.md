# Solución al problema del Teatro de Operaciones usando Hilos

- La solución fue hecha en C

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
./teoph <numero de hilos> <instancia>
```

> [!NOTE]
> El programa asume que el número de procesos hilos es un entero

### Ejecución del programa con Valgrind para comprobar gestión de la memoria y posibles errores

```
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  ./teoph <numero de hilos> <instancia>
```

### En caso de querer compilarlo y ejecutarlo en Windows con gcc (cmd o PowerShell)

```
gcc main.c teatro.c -o teoph
```
```
teoph <numero de hilos> <instancia>
```
