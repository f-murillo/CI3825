#ifndef NODE_H
#define NODE_H
#include <time.h>

enum TYPEFILE {DIR, F}; // Tipo de elemento
// Declaracion de la estructura nodo
typedef struct nodeStruct nodeStruct;

// Funciones de operacion sobre los nodos
nodeStruct* createNode(const char *name, enum TYPEFILE type, nodeStruct *parent);
void deleteNode(nodeStruct *node);
void printNodeDetails(const nodeStruct *node);

// Funciones de acceso y modificacion de los campos de los nodos
nodeStruct* getParent(const nodeStruct *node);
nodeStruct* getChild(const nodeStruct *node);
nodeStruct* getSibling(const nodeStruct *node);
const char* getKey(const nodeStruct *node);
enum TYPEFILE getType(const nodeStruct *node);
time_t getTime(const nodeStruct *node);
void setChild(nodeStruct *parent, nodeStruct *child); // Para establecer un nuevo hijo
void setSibling(nodeStruct *node, nodeStruct *sibling); // Para establecer un nuevo hermano

#endif // NODE_H