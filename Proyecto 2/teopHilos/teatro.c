```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "teatro.h"

// Funcion que procesa los drones del teatro (que procesara cada hilo)
void *process_drone(void *arg){
    ThreadData *data = (ThreadData *)arg; // Se obtienen los datos del hilo
    // Para todos los datos del hilo
    for(int i = data->start; i < data->end; i++){
        Drone drone = data->drones[i]; // Se obtiene el dron
        /*
            Se calculan las coordenadas de la onda explosiva del dron, ajustandolas en 
            caso de que la esquina sup. izquierda tenga coordenadas sean negativas, o 
            si la esquina inf. derecha tiene coordenadas que igualan o superan al numero
            de filas y columnas del teatro
        */
        // Esq. superior izquierda
        int x_min = (drone.x - drone.radius < 0) ? 0 : drone.x - drone.radius;
        int y_min = (drone.y - drone.radius < 0) ? 0 : drone.y - drone.radius;
        // Esq. inferior derecha
        int x_max = (drone.x + drone.radius >= data->rows) ? data->rows : drone.x + drone.radius;
        int y_max = (drone.y + drone.radius >= data->columns) ? data->columns : drone.y + drone.radius;

        // Evaluamos todos los objetos para la onda explosiva
        for(int j = 0; j < data->num_objects; j++){
            Object *obj = &data->objects[j]; // Objeto
            pthread_mutex_lock(&obj->mutex); // Entra en la seccion critica para el objeto
            // Si el objeto no esta destuido, y esta dentro de las coordenadas de la onda explosiva
            if(!obj->destroyed && obj->x >= x_min && obj->x <= x_max &&
                obj->y >= y_min && obj->y <= y_max){
                if(obj->resistance > 0) // Si el objeto es IC
                    obj->resistance -= drone.power; 
                else // Si es OM
                    obj->resistance += drone.power;

                // Verificamos si el objeto fue destruido totalmente
                if((obj->original_resistance > 0 && obj->resistance <= 0) ||
                   (obj->original_resistance < 0 && obj->resistance >= 0)){
                    obj->destroyed = 1;
                } 
            }
            pthread_mutex_unlock(&obj->mutex); // Sale de la seccion critica
        }
    }
    return NULL;
}

// Funcion que ejecuta el teatro de operaciones
void executeT(int n, const char *file){
    // Se lee la instancia del archivo
    FILE *fp = fopen(file, "r");
    if(!fp){
        perror("Error al abrir el archivo");
        return;
    }

    // Se leen las dimensiones del teatro de operaciones, y el numero de objetos
    int N, M, K, L;
    fscanf(fp, "%d %d", &N, &M);
    fscanf(fp, "%d", &K);

    // Se leen los objetos y se inicializan en la estructura
    Object *objects = malloc(K * sizeof(Object));
    for(int i = 0; i < K; i++){
        fscanf(fp, "%d %d %d", &objects[i].x, &objects[i].y, &objects[i].resistance);
        objects[i].original_resistance = objects[i].resistance; // Copiamos resistencia inicial
        objects[i].destroyed = 0; // Ningun objeto esta destruido al inicio
        pthread_mutex_init(&objects[i].mutex, NULL); // Se inicializa el mutex del objeto
    }

    // Se leen los drones y se inicializan en la estructura
    fscanf(fp, "%d", &L); // Numero de drones
    Drone* drones = malloc(L * sizeof(Drone));
    for(int i = 0; i < L; i++){
        fscanf(fp, "%d %d %d %d", &drones[i].x, &drones[i].y, &drones[i].radius, &drones[i].power);
    }

    fclose(fp);

    // En caso de haber ingresado un n invalido, se ajusta
    int nT = (N * M < L) ? N * M : L;
    if(n > nT) n = nT;
    
    int results[6] = {0}; // Resultados que se imprimiran

    // Se crean los hilos
    // Atributos del hilo
    pthread_attr_t threadattr;
    pthread_attr_init(&threadattr);
    // Se asigna memoria para los hilos y sus datos
    pthread_t *threads = malloc(n * sizeof(pthread_t));
    ThreadData *thread_data = malloc(n * sizeof(ThreadData));
    int drones_per_thread = L / n; // Drones por hilo 
    for(int i = 0; i < n; i++){
        // Se inicializan los valores de la estructura
        thread_data[i].start = i * drones_per_thread;
        thread_data[i].end = (i == n - 1) ? L : (i + 1) * drones_per_thread;
        thread_data[i].objects = objects; 
        thread_data[i].num_objects = K;  
        thread_data[i].drones = drones;
        thread_data[i].rows = N;
        thread_data[i].columns = M;
        // Se crea el hilo para el dron
        pthread_create(&threads[i], &threadattr, process_drone, &thread_data[i]);
    }
    
    // Se espera a que terminen los hilos
    for(int i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }

    // Se evalua los estados finales de los objetos
    for(int i = 0; i < K; i++){
        Object *obj = &objects[i]; // Obtenemos el objeto
        if(obj->destroyed){ // Si el objeto esta totalmente destruido
            if(obj->original_resistance < 0)
                results[2]++; // OM totalmente destruido
            else
                results[5]++; // IC totalmente destruido
            
        } else if(obj->original_resistance == obj->resistance){ // Si ninguna explosion afecto al objeto
            if(obj->original_resistance < 0)
                results[0]++; // OM sin destruir
            else
                results[3]++; // IC sin destruir
        } else if(obj->original_resistance < 0) // Si se llega aqui, el objeto esta parcialmente destruido
            results[1]++; // OM parcialmente destruido
        else
            results[4]++; // IC parcialmente destruido
    }

    // Se imprimen los resultados
    printf("OM sin destruir: %d\n", results[0]);
    printf("OM parcialmente destruidos: %d\n", results[1]);
    printf("OM totalmente destruidos: %d\n", results[2]);
    printf("IC sin destruir: %d\n", results[3]);
    printf("IC parcialmente destruidos: %d\n", results[4]);
    printf("IC totalmente destruidos: %d\n", results[5]);

    // Liberamos la memoria de los mutex de los objetos, de las estructuras, y de los atributos de los hilos
    for(int i = 0; i < K; i++){
        pthread_mutex_destroy(&objects[i].mutex);
    }
    free(objects);
    free(drones);
    free(threads);
    free(thread_data);
    pthread_attr_destroy(&threadattr); 
}
```
