#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "file_ops.h"

int main(int argc, char *argv[]){
    // Verificamos que el numero de argumentos sean correctos
    if (argc != 3){
        fprintf(stderr, "Uso: %s [directorio d1] [directorio d2]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Verificamos que ambos argumentos correspondan a directorios validos 
    struct stat st1, st2;
    if (stat(argv[1], &st1) < 0 || !S_ISDIR(st1.st_mode)){
        fprintf(stderr, "Error: '%s' no es un directorio valido.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (stat(argv[2], &st2) < 0 || !S_ISDIR(st2.st_mode)){
        fprintf(stderr, "Error: '%s' no es un directorio valido.\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    
    // Se llama a la funcion que sincroniza los directorios 
    sync_dirs(argv[1], argv[2]);
    
    // Se muestra las estadisticas de transferencia acumuladas durante la sincronizacion
    print_transfer_statistics();
    
    return 0;
}
