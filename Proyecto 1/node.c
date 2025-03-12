#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Definicion  de la estructura nodo
struct nodeStruct {
    enum TYPEFILE tf;      // Tipo del nodo
    nodeStruct *parent;    // Apuntador al nodo padre
    nodeStruct *child;     // Apuntador al primer nodo hijo
    nodeStruct *sibling;   // Apuntador al siguiente nodo hermano
    char *key;             // Nombre del nodo
    time_t creation_time;  // Fecha de creacion del nodo
};

// Funcion para crear un nodo
nodeStruct* createNode(const char *name, enum TYPEFILE type, nodeStruct *parent) {
    // Asignamos memoria para un nuevo nodo
    nodeStruct *node = (nodeStruct*)malloc(sizeof(nodeStruct));
    
    // Si la asignacion de memoria fallo
    if (node == NULL) {
        // Imprimir un mensaje de error y salir del programa
        fprintf(stderr, "Error: No se pudo asignar memoria para el nodo.\n");
        exit(EXIT_FAILURE);
    }

    // Campos del nodo
    node->tf = type;           // Tipo
    node->parent = parent;     // Padre
    node->child = NULL;        // Hijo
    node->sibling = NULL;      // Hermano
    node->key = strdup(name);  // Nombre (se copia el nombre dado)
    node->creation_time = time(NULL); // Fecha

    return node;
}

// Metodo para eliminar un nodo
void deleteNode(nodeStruct *node) {
    if (node == NULL) return;  // Si el nodo es NULL
     // Liberar todos los hijos del nodo de manera recursiva
    nodeStruct *child = node->child;
    while (child != NULL) {
        nodeStruct *next = child->sibling; // Guardamos el siguiente hermano antes de eliminar
        deleteNode(child);
        child = next;
    }   
    
    
    
    free(node->key);           // Liberar la memoria del nombre del nodo
    free(node);                // Liberar la memoria del nodo
}

// Metodo para imprimir detalles de un nodo
void printNodeDetails(const nodeStruct *node) {
    // Si el nodo es NULL
    if (node == NULL) return;
    // Se determina el tipo del nodo
    char *type = (node->tf == DIR) ? "D" : "F";
    // Se crea una cadena para almacenar la fecha y hora de creacion
    char time_str[20];
    // Se convierte el tiempo de creacion a una cadena
    strftime(time_str, sizeof(time_str), "%H:%M %d-%m-%Y", localtime(&node->creation_time));
    // Se imprimen los detalles del nodo
    printf("%s %s %s\n", node->key, time_str, type);
}

// Funciones de  acceso y modificacion de campos
nodeStruct* getParent(const nodeStruct *node){return node->parent;}

nodeStruct* getChild(const nodeStruct *node){return node->child;} 

nodeStruct* getSibling(const nodeStruct *node){return node->sibling;}

const char* getKey(const nodeStruct *node) {return node->key;}

enum TYPEFILE getType(const nodeStruct *node){return node->tf;}

time_t getTime(const nodeStruct *node){return node->creation_time;}  

void setChild(nodeStruct *parent, nodeStruct *child){parent->child = child;}

void setSibling(nodeStruct *node, nodeStruct *sibling){node->sibling = sibling;}

