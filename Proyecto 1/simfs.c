#include <stdio.h>
#include <string.h>
#include "node.h"

// Declaramos las funciones de filesystem.c que  se usaran en el menu
void initFileSystem();
void loadFileSystem(const char *filename);
void touch(const char *name);
void mkdir(const char *name);
void cd(const char *name);
void ls(const char *dir, int long_format);
void rm(const char *name);
void rmdir(const char *name);
void pwd(const char *name);
void wrts(const char *filename);
void help();
void exitSystem();
void deleteNode(nodeStruct *node);

// Metodo principal
int main(int argc, char *argv[]){
    char command[512]; // Comando completo
    char cmd[10];      // Comando especifico
    char arg1[256];    // Primer argumento
    char arg2[256];    // Segundo argumento (solo se usara si se quiere listar un directorio con detalles)

    // Se inicializa el sistema de archivos
    initFileSystem();
    // En caso de ser pasado un archivo, se carga
    if (argc > 1) loadFileSystem(argv[1]);
     
    // Menu interactivo
    while (1) {
        printf("> ");
        // Se verifica si no hay algun error con la entrada
        if (fgets(command, sizeof(command), stdin) == NULL) break;
        // Se elimina el salto de linea que agrega fgets al final de la entrada
        command[strcspn(command, "\n")] = '\0';
        // Se escanea el comando del usuario, se separan los argumentos, y se obtiene el numero de argumentos 
        int numArgs = sscanf(command, "%s %s %s", cmd, arg1, arg2);
        // Se procesa el comando ingresado por el usuario
        switch (numArgs) {
            case 1: // Si el comando solo tiene un argumento
                if(strcmp(cmd, "ls") == 0) ls(NULL, 0); // Solo se quiere listar el directorio actual

                else if(strcmp(cmd, "help") == 0) help();
                   
                else if(strcmp(cmd, "pwd") == 0) pwd(NULL); 
                    
                else if(strcmp(cmd, "exit") == 0){
                        exitSystem();
                        printf("Saliendo del programa\n");
                        return 0;
                }

                else
                    printf("Comando no reconocido. Usa 'help' para ver los comandos disponibles.\n");
                
                break;

            case 2: // Si el comando tiene 2 argumentos
                if (strcmp(cmd, "touch") == 0) touch(arg1);

                else if (strcmp(cmd, "rm") == 0) rm(arg1);

                else if (strcmp(cmd, "mkdir") == 0) mkdir(arg1);

                else if (strcmp(cmd, "rmdir") == 0) rmdir(arg1);

                else if (strcmp(cmd, "cd") == 0) cd(arg1);

                else if (strcmp(cmd, "pwd") == 0) pwd(arg1);

                else if (strcmp(cmd, "wrts") == 0) wrts(arg1);

                else if (strcmp(cmd, "ls") == 0) {
                    // Si se quiere listar con detalles los elementos del directorio actual
                    if (strcmp(arg1, "-l") == 0) ls(NULL, 1); 
                    // Si no, se quiere listar los elementos de un directorio dado    
                    else ls(arg1, 0); 
                } else // Comando equivocado
                    printf("Comando no reconocido. Usa 'help' para ver los comandos disponibles.\n");
                
                break;

            case 3: // El unico caso donde habra 3 argumentos sera si se quiere listar los elementos de un directorio dado con detalles
                if (strcmp(cmd, "ls") == 0 && strcmp(arg1, "-l") == 0)
                    ls(arg2, 1);
                else
                    printf("Comando no reconocido. Usa 'help' para ver los comandos disponibles.\n");
                
                break;
            default: // Si se ingrasa un comando equivocado
                printf("Comando no reconocido. Usa 'help' para ver los comandos disponibles.\n");
                break;
        }
    }

    printf("Saliendo del programa\n");
    return 0;
}
