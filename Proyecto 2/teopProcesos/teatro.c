#include "teatro.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

void process_drones(int start, int end, Object *objects, int num_objects, Drone *drones, int rows, int columns, sem_t *sem) {
    for (int i = start; i < end; i++) {
        Drone drone = drones[i];
        
        // Calcular los límites de la onda explosiva
        int x_min = (drone.x - drone.radius < 0) ? 0 : drone.x - drone.radius;
        int y_min = (drone.y - drone.radius < 0) ? 0 : drone.y - drone.radius;
        int x_max = (drone.x + drone.radius >= rows) ? rows  : drone.x + drone.radius;
        int y_max = (drone.y + drone.radius >= columns) ? columns : drone.y + drone.radius;
        
       // Iterar sobre los objetos y verificar si están dentro del área afectada por el dron
        for (int j = 0; j < num_objects; j++) {
            Object *obj = &objects[j];
            
            sem_wait(sem); // Inicio de sección crítica
            
             // Verificar si el objeto está dentro del rango de la onda explosiva
            if (!obj->destroyed && obj->x >= x_min && obj->x <= x_max && obj->y >= y_min && obj->y <= y_max) {
                if (obj->resistance > 0) // Si es IC
                    obj->resistance -= drone.power;// Restar resitancia IC con poder del dron 
                else // Si es OM
                    obj->resistance += drone.power;// Sumar resitancia OM con poder del dron 
                
                // Verificar si el objeto ha sido destruido
                if ((obj->original_resistance > 0 && obj->resistance <= 0) ||
                    (obj->original_resistance < 0 && obj->resistance >= 0)) {
                    obj->destroyed = 1;
                }
            }
            
            sem_post(sem); // Fin de sección crítica
        }
    }
}

// Función  que ejecuta la simulación del teatro de operaciones
void ejecutar_teatro(int n, const char *file) {
    // Abrir el archivo y manejar errores
    FILE *fp = fopen(file, "r");
    if (!fp) {
        perror("Error al abrir el archivo");
        exit(1);
    }
     // Leer las dimensiones del teatro de operaciones (N x M) y el número de objetos (K)
    int N, M, K, L;
    fscanf(fp, "%d %d", &N, &M); // Lectura de N y M
    fscanf(fp, "%d", &K); // Lectura de número de objetos

    // Crear memoria compartida para los objetos
    Object *objects = mmap(NULL, K * sizeof(Object), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (objects == MAP_FAILED) {
        perror("Error al crear memoria compartida para objetos");
        exit(1);
    }
    
    // Leer los objetos desde el archivo
    for (int i = 0; i < K; i++) {
        fscanf(fp, "%d %d %d", &objects[i].x, &objects[i].y, &objects[i].resistance);
        objects[i].original_resistance = objects[i].resistance;
        objects[i].destroyed = 0;
    }
    // Leer el número de drones (L)
    fscanf(fp, "%d", &L);
    if (L <= 0) {
        fprintf(stderr, "Error: Número de drones inválido (%d)\n", L);
        exit(1);
    }
    // Crear memoria compartida para los drones
    Drone *drones = mmap(NULL, L * sizeof(Drone), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (drones == MAP_FAILED) {
        perror("Error al crear memoria compartida para drones");
        exit(1);
    }
    // Leer los drones desde el archivo
    for (int i = 0; i < L; i++) {
        fscanf(fp, "%d %d %d %d", &drones[i].x, &drones[i].y, &drones[i].radius, &drones[i].power);
    }
    fclose(fp);
    
    // Calcular el valor máximo permitido para n (nT)
    int nT = (N * M < L) ? N * M : L;
    if (n > nT) {
        printf("Número de procesos excede el límite permitido. Se usará nT.\n");
        n = nT;
    }
        // Crear un semáforo para la sincronización de procesos
    sem_t *sem = sem_open("sem_obj", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("Error al crear semáforo");
        exit(1);
    }
    
    // Dividir los drones entre los procesos
    int drones_per_process = (L + n - 1) / n;
    // Crear procesos hijos
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Proceso hijo
            int start = i * drones_per_process; // Índice de inicio del rango de drones
            int end = (i + 1) * drones_per_process; // Índice final del rango de drones
            if (end > L) end = L;
            process_drones(start, end, objects, K, drones, N, M, sem);
            exit(0);
        }
    }
    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
 
 /* Crear un arreglo 'results' para almacenar las cantidades de objetos en cada estado posible
Los estados son: sin destruir, parcialmente destruidos, totalmente destruidos
Se usan índices para almacenar diferentes tipos de objetos y su estado:
results[0] -> OM sin destruir
results[1] -> OM parcialmente destruidos
results[2] -> OM totalmente destruidos
results[3] -> IC sin destruir
results[4] -> IC parcialmente destruidos
results[5] -> IC totalmente destruidos */
     
     // Imprimir resultados
    int results[6] = {0};
    // Iteramos sobre todos los objetos para verificar su estado después del ataque
    for (int i = 0; i < K; i++) {
        Object *obj = &objects[i];
        if (obj->destroyed) {
            if (obj->original_resistance < 0)
                results[2]++;
            else
                results[5]++;
        } else if (obj->original_resistance == obj->resistance) {
            if (obj->original_resistance < 0)
                results[0]++;
            else
                results[3]++;
        } else if (obj->original_resistance < 0)
            results[1]++;
        else
            results[4]++;
    }
    printf("Resultados del ataque:\n");
    printf("OM sin destruir: %d\n", results[0]);
    printf("OM parcialmente destruidos: %d\n", results[1]);
    printf("OM totalmente destruidos: %d\n", results[2]);
    printf("IC sin destruir: %d\n", results[3]);
    printf("IC parcialmente destruidos: %d\n", results[4]);
    printf("IC totalmente destruidos: %d\n", results[5]);
    
    // Liberar recursos del semáforo y memoria compartida
    
    sem_close(sem);
    
    // Elimina el semáforo del sistema, liberando el espacio que ocupaba
    sem_unlink("sem_obj");
 
    // `munmap` desvincula la memoria compartida asignada a `objects`, liberando el recurso
    munmap(objects, K * sizeof(Object)); // Libera el espacio de memoria donde se almacenan los objetos
    
    // `munmap` desvincula la memoria compartida asignada a `drones`, liberando el recurso
    munmap(drones, L * sizeof(Drone)); // Libera el espacio de memoria donde se almacenan los drones
}

