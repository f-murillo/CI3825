#include <stdio.h>
#include <stdlib.h>
#include "teatro.h"

// Funcion principal
int main(int argc, char *argv[]){
    if(argc != 3){ // En caso de ingresar mas o menos de 3 argumentos
        printf("Error: numero de argumentos invalido. Ejemplo: %s [n] [archivo_instancia]\n", argv[0]);
        return 1;
    }
    // Numero de hilos ingresado por el usuario, y el archivo con la instancia
    int n = atoi(argv[1]); 
    char *file = argv[2];
    // Se ejecuta el teatro de operaciones
    executeT(n, file);
    return 0;
}