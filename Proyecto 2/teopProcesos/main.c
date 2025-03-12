#include "teatro.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Verifica que se hayan pasado los argumentos correctos
    if (argc != 3) {
        printf("Error: número de argumentos inválido. Uso: %s [n] [archivo_instancia]\n", argv[0]);
        return 1;// Retorna error si los argumentos son incorrectos
    }
    // Convierte el primer argumento (número de procesos) a entero
    int n = atoi(argv[1]);
    // Obtiene el nombre del archivo de instancia desde los argumentos
    char *file = argv[2];
    // Llama a la función que ejecuta la simulación del teatro de operaciones
    ejecutar_teatro(n, file);
    

    return 0;
}

