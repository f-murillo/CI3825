#ifndef TEATRO_H
#define TEATRO_H

#include <semaphore.h>

// Definición de las estructuras
typedef struct {
    int x, y;	              // Coordenadas (x, y) del objeto en el teatro de operaciones
    int resistance;           // Resistencia actual del objeto (puede cambiar durante la simulación)
    int original_resistance;  // Resistencia original del objeto (valor inicial antes de recibir daño)
    int destroyed;            // Estado del objeto: 1 si está destruido, 0 si no lo está
} Object;

typedef struct {
    int x;      // Coordenada x del dron
    int y;      // Coordenada y del dron
    int radius; // Radio de acción del dron (área afectada por su onda explosiva)
    int power;  // Potencia del dron (intensidad de la onda explosiva)
} Drone;


// Declaración de funciones
void process_drones(int start, int end, Object *objects, int num_objects, Drone *drones, int rows, int columns, sem_t *sem);
void ejecutar_teatro(int n, const char *file);

#endif



