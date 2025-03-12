#ifndef TEATRO_H
#define TEATRO_H

// Structs para procesar la informacion con los hilos
// Struct Objeto
typedef struct{
    int x, y;                   // Coordenadas del objeto en la cuadricula
    int resistance;            // Resistencia del objeto (OM o IC)
    int original_resistance;  // Resistencia original (para verificar si el objeto fue destruido o no)
    int destroyed;             // Indicador de destruccion total del objeto (1 si lo esta, 0 si no lo esta)
    pthread_mutex_t mutex;     // Mutex asociado al objeto
} Object;

// Struct Dron
typedef struct{
    int x, y;       // Coordenadas de impacto del dron
    int radius;     // Radio de destruccion (RD)
    int power;      // Poder explosivo (PE)
} Drone;

// Struct para los datos que del hilo
typedef struct{
    int start;          // Indice inicial para drones
    int end;             // Indice final para drones
    Object *objects;     // Objetos
    int num_objects;
    Drone *drones;        // Drones
    int rows;           // Numero de filas del teatro de operaciones
    int columns;        // Numero de columnas
} ThreadData;

// Funciones que haran uso de las estructura
void *process_drone(void *arg);
void executeT(int n, const char *file);

#endif