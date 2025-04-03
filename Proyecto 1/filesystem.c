#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"

// Nodo raiz y nodo actual en el sistema de archivos
nodeStruct *root = NULL;
nodeStruct *current = NULL;

// Metodo para inicializar el sistema de archivos
void initFileSystem(){
    // Se crea el nodo raiz, y se establece el nodo actual como la raiz
    root = createNode("/", DIR, NULL);
    current = root;
}

/*
    Funcion auxiliar para navegar hasta un directorio, dado un camino absoluto o relativo 
    (para poder ingresar los caminos como argumentos). Usada por verifyPath, addElement,
    y removeElement
*/
nodeStruct* browseToDirectory(const char *path){
    // Si la ruta es absoluta, comienza desde la raiz; si no, desde el directorio actual
    nodeStruct *directory = (path[0] == '/') ? root : current;
    char *token;
    char *pathCopy = strdup(path); // Se copia la ruta para tokenizacion
    char *rest = pathCopy;         
    token = strtok_r(rest, "/", &rest); // Tokeniza la ruta (separa cada parte de la ruta) usando "/" como el separador

    // Mientras hayan tokens
    while(token != NULL){
        // Si se encuentra "..", se sube al directorio padre
        if(strcmp(token, "..") == 0){
            directory = (getParent(directory) != NULL) ? getParent(directory) : directory;
        }
        // Si no, se busca el directorio hijo correspondiente
        else{
            nodeStruct *child = getChild(directory);
            int found = 0; // Para indicar si se encontro o no el directorio hijo
            // Mientras haya hijos
            while(child != NULL){
                // Si el hijo es un directorio, y su nombre corresponde al token actual
                if(getType(child) == DIR && strcmp(getKey(child), token) == 0){
                    directory = child;
                    found = 1;
                    break;
                }
                child = getSibling(child);
            }
            // Si no se encuentra el directorio, libera la memoria de la copia de la ruta y retorna NULL
            if(!found){
                free(pathCopy);
                return NULL;
            }
        }
        // Se pasa al siguiente token
        token = strtok_r(rest, "/", &rest);
    }

    // Se libera la memoria de la copia de la ruta y retorna el directorio encontrado
    free(pathCopy); 
    return directory; 
}

// Funcion que verifica si un camino es valido (usada por addElement y removeElement)
int verifyPath(const char *path, char **name, nodeStruct **targetDir){
    char *pathCopy = strdup(path); // Se crea una copia del camino
    char *elementName = strrchr(pathCopy, '/'); // Se buscaa la ultima aparicion de "/" en la copia del camino
    
    // Si hay "/" en el camino
    if(elementName != NULL){
        *elementName = '\0'; // Se separa el camino del nombre del elemento, terminando la cadena en la posicion del "/"
        elementName++; // Se avanza el puntero una posicion, para que elementName apunte al nombre del elemento
        *targetDir = browseToDirectory(pathCopy); // Se navega al directorio dado en el camino
    } else{ // Si no hay "/", el directorio destino es el directorio actual
        *targetDir = current;
        elementName = pathCopy; // El nombre del elemento es la ruta completa
    }

    // Se verifica si el camino especificado es valida
    if(*targetDir == NULL){
        printf("Error: la ruta '%s' no es valida.\n", path);
        free(pathCopy); // Se libera la memoria de la copia del camino
        return 0; // La verificacion fallo
    }

    *name = strdup(elementName); // Se copia el nombre del elemento
    free(pathCopy); // Se libera la memoria de la copia de la ruta
    return 1; // La verificacion fue exitosa
}


// Metodo para agregar un elemento (usado por touch y mkdir)
void addElement(const char *path, enum TYPEFILE type){
    char *name; // Nombre del elemento
    nodeStruct *targetDir; // Directorio destino

    // Si el camino no es valido
    if(!verifyPath(path, &name, &targetDir)) return; 

    // Se verifica si ya existe un archivo o directorio con el mismo nombre en el directorio destino
    nodeStruct *child = getChild(targetDir); // Primer hijo del directorio destino
    nodeStruct *prev = NULL; // Nodo anterior al hijo
    while(child != NULL){
        if(strcmp(getKey(child), name) == 0){
            printf("Error: El archivo o directorio '%s' ya existe.\n", name);
            free(name);
            return;
        }
        // Se actualiza prev como el hijo actual, y se actualiza el hijo actual como su hermano
        prev = child;
        child = getSibling(child);
    }

    // Si se llego a este punto, no existe otro elemento en el directorio con el mismo nombre, asi que se crea
    nodeStruct *newElement;
    if(type == F) // Si es un archivo
        newElement = createNode(name, F, targetDir); 
    else // Si es un directorio
        newElement = createNode(name, DIR, targetDir); 
    
    // Agregar el nuevo elemento al sistema de archivos
    if(prev == NULL)
        setChild(targetDir, newElement); // Si no hay hijos en el directorio destino, agrega el elemento como primer hijo
    else
        setSibling(prev, newElement); // Si ya hay hijos, agrega el nuevo elemento como el siguiente hermano del ultimo hijo

    printf("%s '%s' creado correctamente.\n", type == DIR ? "Directorio" : "Archivo", name);
    free(name); // Liberamos la memoria del nombre del elemento
}
// Metodo para crear un nuevo archivo
void touch(const char *path){addElement(path, F);}
// Metodo para crear un nuevo directorio
void mkdir(const char *path){addElement(path, DIR);}


int silentMode = 0;  // 0: modo normal, 1: modo silencioso
// Metodo que carga el sistema de archivos desde un archivo dado
void loadFileSystem(const char *filename){
    FILE *file = fopen(filename, "r");
   if(!file){
        printf("Error: No se pudo abrir el archivo %s\n", filename);
        return;
    }
    
    // Se activa el modo silencioso
    silentMode = 1;
    char path[256], type; // Variables para el camino y el tipo del elemento

    // Mientras haya lineas en el archivo
    while(fscanf(file, "%s\t%c", path, &type) != EOF){
        /*
            Se ignora la primera linea (que contiene el directorio raiz "/", 
            ya que al iniciar el programa, ya se crea el directorio raiz)
        */
        if (strcmp(path, "/") == 0 && type == 'D') continue;
       // Se crea el elemento de la linea del archivo
       if(type == 'D') mkdir(path); // Crear directorio
       else touch(path); // Crear archivo
    }
    // Se cierra el archivo y se desactiva el modo silencioso
    fclose(file);
    silentMode = 0;
}

// Metodo para eliminar un elemento (usado por rm y rmdir)
void removeElement(const char *path, enum TYPEFILE type){
    char *name; // Nombre del elemento
    nodeStruct *targetDir; // Directorio destino

    // Si el camino no es valido
    if(!verifyPath(path, &name, &targetDir)) return;

    /*
        Se verifica si el elemento a eliminar tiene el tipo esperado, y si en caso de 
        querer eliminar un directorio, este esta vacio
    */
    nodeStruct *child = getChild(targetDir); // Primer hijo del directorio destino
    nodeStruct *prev = NULL; // Nodo anterior al hijo actual
    while(child != NULL){
        // Si se encuentra un nodo con el nombre especificado
        if(strcmp(getKey(child), name) == 0){
            // Verificar si el tipo del nodo coincide con el tipo esperado
            if(getType(child) != type){
                printf("Error: '%s' no es del tipo esperado.\n", name);
                free(name);
                return;
            }
            // En caso de eliminar un directorio, verificar si está vacío
            if(type == DIR && getChild(child) != NULL){
                printf("Error: El directorio '%s' no está vacío.\n", name);
                free(name);
                return;
            }
            // Si se llego a este punto, se elimina el nodo
            if(prev == NULL)
                // Si el nodo a eliminar es el primer hijo
                setChild(targetDir, getSibling(child));
            else
                // Si el nodo a eliminar no es el primer hijo
                setSibling(prev, getSibling(child));

            deleteNode(child); // Se libera la memoria ocupada por el nodo
            printf("%s '%s' eliminado.\n", type == DIR ? "Directorio" : "Archivo", name);
            free(name); // Se libera la memoria del nombre del elemento
            return;
        }
        // Se actualiza prev como el hijo actual, y se actualiza el hijo actual como su hermano
        prev = child;
        child = getSibling(child);
    }

    // Si no se encuentra el nodo, imprimir mensaje de error
    printf("Error: No se encontró '%s'.\n", name);
    free(name); // Liberar la memoria del nombre del elemento
}
// Metodo para eliminar un archivo
void rm(const char *name){removeElement(name, F);}
// Metodo para eliminar un directorio
void rmdir(const char *name){removeElement(name, DIR);}

// Metodo para cambiar de directorio
void cd(const char *name){
    // Si se intenta cambiar al directorio actual(".")
    if(strcmp(name, ".") == 0){
        printf("Ya estas en '%s'\n", getKey(current));
        return;
    }
    nodeStruct *targetDirectory = browseToDirectory(name); // Se navega hasta el directorio
    if(targetDirectory != NULL){ // Si se encontro el directorio
        current = targetDirectory;
        printf("Ahora estas en '%s'\n", getKey(current));
    } else{
        printf("Error: No se encontro el directorio '%s'.\n", name);
    }
}

// Metodo para listar archivos y directorios
void ls(const char *dir, int long_format){
    nodeStruct* directory; // Directorio a listar

    if(dir == NULL) // Si no se especifica un directorio, se lista el directorio actual
        directory = current;
    else // Si no, se navega hasta el directorio especificado
        directory = browseToDirectory(dir);
    
    // Si existe el directorio
    if(directory != NULL){
        // Se listan los elementos del directorio
        nodeStruct *child = getChild(directory); // Hijo del directorio
        while(child != NULL){
            // Se imprimen los detalles del nodo si long_format es verdadero
            if(long_format)
                printNodeDetails(child);
            else
                // Se imprime solo el nombre del nodo si long_format es falso
                printf("%s  ", getKey(child));

            // Se pasa al elemento hermano
            child = getSibling(child);
        }
        printf("\n");
    } else{
        printf("Error: No se encontro el directorio '%s'.\n", dir);
    }
}

// Función auxiliar para obtener el camino absoluto desde la raíz hasta un nodo dado
void getAbsolutePath(nodeStruct *node, char *path){
    nodeStruct *stack[256]; // Pila para almacenar los nombres de nodos
    int top = -1;

    // Subir hasta la raíz y almacenar los nodos en la pila
    while(node != root){
        stack[++top] = node;
        node = getParent(node);
    }

    // Construir el camino absoluto desde la raíz
    strcpy(path, "/");
    while(top >= 0){
        strcat(path, getKey(stack[top--]));
       if(top >= 0) strcat(path, "/");
    }
}

// Metodo para imprimir el camino absoluto desde la raíz hasta el directorio actual
void pwd(){
    char path[256];
    getAbsolutePath(current, path);
    printf("%s\n", path);
}

// Metodo para recorrer el arbol en preorder y escribir en el archivo creado la informacion de cada nodo
void writePreorder(FILE *file, nodeStruct *node){
   if(node == NULL) return;

    char path[256];
   if(getType(node) == F){
        getAbsolutePath(getParent(node), path);
        // Agregamos "/nombre_archivo" al final del camino absoluto del padre
        strcat(path, "/");
        strcat(path, getKey(node)); 
    } else{
        getAbsolutePath(node, path);
    }

    // Verificamos si el nombre del nodo es válido antes de imprimir
    const char* key = getKey(node);
   if(key != NULL && strlen(key) > 0){
        // Nombre del elemento
        fprintf(file, "%s\t", key);
        
        // Fecha de creación
        char time_str[20];
        time_t timeC = getTime(node);
        strftime(time_str, sizeof(time_str), "%H:%M %d-%m-%Y", localtime(&timeC)); // Para que tenga el mismo formato que en printNodeDetails
        fprintf(file, "%s\t", time_str);

        // Tipo de elemento
        fprintf(file, "%c\t", (getType(node) == F) ? 'F' : 'D');

        // Camino absoluto
        fprintf(file, "%s\n", path);
    }

    // Recorrer los hijos del nodo
    nodeStruct *child = getChild(node);
    while(child != NULL){
        writePreorder(file, child);
        child = getSibling(child);
    }
}

// Metodo que crea un archivo con la descripcion del sistema de archivos hasta el momento
void wrts(const char *filename){
    FILE *file = fopen(filename, "w");
   if(file == NULL){
        printf("Error: No se pudo crear el archivo '%s'.\n", filename);
        return;
    }

    // Iniciar el recorrido preorden desde la raíz
    writePreorder(file, root);

    fclose(file);
    printf("Archivo '%s' creado.\n", filename);
}


// Metodo para mostrar la ayuda
void help(){
    printf("Comandos disponibles:\n");
    printf("touch <nombre_archivo>   - Crea un nuevo archivo en el directorio actual, o en un directorio de un camino dado\n");
    printf("rm <nombre_archivo>      - Borra un archivo en el directorio actual, o en un directorio de un camino dado\n");
    printf("mkdir <nombre_directorio> - Crea un directorio dentro del directorio actual, o en un directorio de un camino dado\n");
    printf("rmdir <nombre_directorio> - Borra un directorio vacio dentro del directorio actual o en un directorio de un camino dado\n");
    printf("ls [-l]                  - Lista los archivos y directorios del directorio actual y los directorios hijo, o de un directorio de un camino dado\n");
    printf("cd <nombre_directorio>   - Cambiar de directorio a los directorios hijos, al directorio padre, a la raiz, o a un directorio en un camino dado\n");
    printf("pwd                      - Imprime el camino absoluto hasta el directorio actual\n");
    printf("wrts <nombre_archivo>    - Crea un archivo con el nombre dado, el cual contiene todos los elementos del sistema de archivos.");
    printf("help                     - Muestra la guia de ayuda\n");
    printf("exit                     - Sale del programa\n");
}

// Metodo que sale del sistema, donde se elimina la raiz del arbol, liberandolo de la memoria
void exitSystem(){deleteNode(root);}
