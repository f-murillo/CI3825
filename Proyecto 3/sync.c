#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "file_ops.h"

// Variables globales para las estadisticas de transferencia de archivos
static unsigned long files_d1_to_d2 = 0;
static unsigned long kilobytes_d1_to_d2 = 0;
static unsigned long files_d2_to_d1 = 0;
static unsigned long kilobytes_d2_to_d1 = 0;

/*
  Funcion que acumula el numero de archivos regulares y la suma en bytes de sus tamaños
  Retorna 0 en caso de exito o -1 en caso de error
 */
static int sum_dir_files(const char *dir_path, unsigned long *file_count, unsigned long *total_bytes){
    DIR *dir = opendir(dir_path);
    if (!dir){
        perror("sum_dir_files: error abriendo directorio");
        return -1;
    }
    struct dirent *entry;
    char path[PATH_MAX];
    struct stat st;

    while ((entry = readdir(dir)) != NULL){
        if ( strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )
            continue;
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        if (stat(path, &st) < 0){
            perror("sum_dir_files: error en stat");
            continue;
        }
        if (S_ISREG(st.st_mode)){
            (*file_count)++;
            *total_bytes += st.st_size;
        } else if (S_ISDIR(st.st_mode)){
            sum_dir_files(path, file_count, total_bytes);
        }
    }
    closedir(dir);
    return 0;
}

// Funcion que sincroniza el contenido de dos directorios
void sync_dirs(const char *d1, const char *d2){
    DIR *dir1 = opendir(d1);
    if (!dir1){
        perror("sync_dirs: error abriendo d1");
        return;
    }
    struct dirent *entry;
    struct stat st1, st2;
    char path1[PATH_MAX], path2[PATH_MAX];
    char decision;
    int opcion_valida;

    // Se recorren las entradas del directorio d1 
    while ((entry = readdir(dir1)) != NULL){
        if ( strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )
            continue;
        snprintf(path1, sizeof(path1), "%s/%s", d1, entry->d_name);
        snprintf(path2, sizeof(path2), "%s/%s", d2, entry->d_name);

        if (stat(path1, &st1) < 0){
            perror("sync_dirs: error en stat de d1");
            continue;
        }

        // Si el elemento no existe en d2 
        if (stat(path2, &st2) < 0){
            printf("El elemento '%s' existe en %s pero no en %s.\n", entry->d_name, d1, d2);
            
            opcion_valida = 0;
            while(!opcion_valida){
                printf("Desea copiarlo a %s (c) o borrarlo de %s (b)? ", d2, d1);
                scanf(" %c", &decision);
                switch(decision){
                    case 'c':
                        opcion_valida = 1;
                        // Se copia de d1 hacia d2 
                        if (S_ISDIR(st1.st_mode)){
                            if (cp_dir_to_dir(path1, d2) == 0){
                                /* Actualiza las estadísticas; se recorre el directorio copiado para acumular datos */
                                unsigned long count = 0, bytes = 0;
                                sum_dir_files(path1, &count, &bytes);
                                files_d1_to_d2 += count;
                                kilobytes_d1_to_d2 += bytes / 1024;
                                printf("Directorio '%s' copiado de %s a %s.\n", entry->d_name, d1, d2);
                            }
                        }
                        else if (S_ISREG(st1.st_mode)){
                            if (cp_file_to_dir(path1, d2) == 0){
                                files_d1_to_d2++;
                                kilobytes_d1_to_d2 += st1.st_size / 1024;
                                printf("Archivo '%s' copiado de %s a %s.\n", entry->d_name, d1, d2);
                            }
                        }
                        break;
                    case 'b':
                        // Se borra en d1 
                        opcion_valida = 1;
                        if (S_ISDIR(st1.st_mode)){
                            if (rm_dir(path1) == 0 && rmdir(path1) == 0)
                                printf("Directorio '%s' borrado de %s.\n", entry->d_name, d1);
                        }
                        else if (S_ISREG(st1.st_mode)){
                            if (remove(path1) == 0)
                                printf("Archivo '%s' borrado de %s.\n", entry->d_name, d1);
                        }
                        break;
                    default:
                        printf("Error: Opcion ingresada no valida. Ingresa c o b.\n");
                        break; 
                }
            }            
        }else{ // El elemento existe en ambos directorios
            if (S_ISREG(st1.st_mode) && S_ISREG(st2.st_mode)){
                // Caso de archivos: Comprobar si son iguales 
                // Si son iguales no hacemos nada
                if (st1.st_size == st2.st_size && same_content_file(path1, path2)) 
                    continue; 
                else{
                    // Si son diferentes, se comparan sus fechas de modificacion 
                    printf("\nEl archivo '%s' difiere en %s y %s.\n", entry->d_name, d1, d2);

                    // Formateamos la fecha de modificacion para d1
                    time_t mod_time1 = st1.st_mtime;
                    struct tm *tm_info1 = localtime(&mod_time1);
                    char time_str1[26];  // Buffer para la fecha en formato "YYYY-MM-DD HH:MM:SS"
                    strftime(time_str1, sizeof(time_str1), "%Y-%m-%d %H:%M:%S", tm_info1);
                    
                    // Para d2
                    time_t mod_time2 = st2.st_mtime;
                    struct tm *tm_info2 = localtime(&mod_time2);
                    char time_str2[26];
                    strftime(time_str2, sizeof(time_str2), "%Y-%m-%d %H:%M:%S", tm_info2);
                    
                    printf("Fecha de modificacion en %s: %s\n", d1, time_str1);
                    printf("Fecha de modificacion en %s: %s\n", d2, time_str2);
                    printf("Se recomienda copiar el archivo con fecha mas reciente.\n");
                    
                    int opcion;
                    opcion_valida = 0; 
                    while(!opcion_valida){ // Mientras la opcion no sea valida
                        printf("Cual version desea mantener? (1: version en %s, 2: version en %s): ", d1, d2);
                        scanf("%i", &opcion);
                        switch(opcion){
                            case 1: // Mantenemos la version de d1; se borra el archivo en d2 y se copia desde d1
                                opcion_valida = 1;
                                if (remove(path2) == 0){
                                    if (cp_file_to_dir(path1, d2) == 0){
                                        files_d1_to_d2++;
                                        kilobytes_d1_to_d2 += st1.st_size / 1024;
                                        printf("Archivo '%s' actualizado en %s.\n", entry->d_name, d2);
                                    }
                                }
                                break;
                            case 2: // Mantenemos la version de d2; se borra el archivo en d1 y se copia desde d2
                                opcion_valida = 1;
                                if (remove(path1) == 0){
                                    if (cp_file_to_dir(path2, d1) == 0){
                                        files_d2_to_d1++;
                                        kilobytes_d2_to_d1 += st2.st_size / 1024;
                                        printf("Archivo '%s' actualizado en %s.\n", entry->d_name, d1);
                                    }
                                }
                                break;
                            default:
                                printf("Error: Opcion ingresada no valida.\n");
                                break;
                        }
                    }
                }
            }
            else if (S_ISDIR(st1.st_mode) && S_ISDIR(st2.st_mode)){
                //Si ambos son directorios, se sincronizan recursivamente 
                sync_dirs(path1, path2);
            }
            else{
                // En caso de que uno es directorio y el otro archivo 
                printf("Conflicto en '%s': uno es directorio y el otro un archivo. No se puede sincronizar.\n", entry->d_name);
            }
        }
    }
    closedir(dir1);

    // Procesamos las entradas del directorio d2 que no esten en d1 
    DIR *dir2 = opendir(d2);
    if (!dir2){
        perror("sync_dirs: error abriendo d2");
        return;
    }
    while ((entry = readdir(dir2)) != NULL){
        if ( strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )
            continue;
        snprintf(path2, sizeof(path2), "%s/%s", d2, entry->d_name);
        snprintf(path1, sizeof(path1), "%s/%s", d1, entry->d_name);

        if (stat(path2, &st2) < 0){
            perror("sync_dirs: error en stat de d2");
            continue;
        }
        // Si el elemento no existe en d1 
        if (stat(path1, &st1) < 0){
            printf("El elemento '%s' existe en %s pero no en %s.\n", entry->d_name, d2, d1);

            opcion_valida = 0;
            while(!opcion_valida){
                printf("Desea copiarlo a %s (c) o borrarlo de %s (b)? ", d1, d2);
                scanf(" %c", &decision);

                switch(decision){
                    case 'c':
                        opcion_valida = 1;
                        if (S_ISDIR(st2.st_mode)){
                            if (cp_dir_to_dir(path2, d1) == 0){
                                unsigned long count = 0, bytes = 0;
                                sum_dir_files(path2, &count, &bytes);
                                files_d2_to_d1 += count;
                                kilobytes_d2_to_d1 += bytes / 1024;
                                printf("Directorio '%s' copiado de %s a %s.\n", entry->d_name, d2, d1);
                            }
                        }
                        else if (S_ISREG(st2.st_mode)){
                            if (cp_file_to_dir(path2, d1) == 0){
                                files_d2_to_d1++;
                                kilobytes_d2_to_d1 += st2.st_size / 1024;
                                printf("Archivo '%s' copiado de %s a %s.\n", entry->d_name, d2, d1);
                            }
                        }
                        break; 
                    case 'b':
                        opcion_valida = 1;
                        if (S_ISDIR(st2.st_mode)){
                            if (rm_dir(path2) == 0 && rmdir(path2) == 0)
                                printf("Directorio '%s' borrado de %s.\n", entry->d_name, d2);
                        }
                        else if (S_ISREG(st2.st_mode)){
                            if (remove(path2) == 0)
                                printf("Archivo '%s' borrado de %s.\n", entry->d_name, d2);
                        }
                        break;
                    default:
                        printf("Error: Opcion ingresada no valida.");   
                }
            }
        }
    }
    closedir(dir2);
}

// Funcion que Muestra por salida estandar la cantidad de archivos y kilobytes transferidos en cada sentido.
void print_transfer_statistics(){
    printf("\nEstadisticas de Transferencia:\n");
    printf("De %s a %s: %lu archivos, %lu kilobytes transferidos.\n", 
           "d1", "d2", files_d1_to_d2, kilobytes_d1_to_d2);
    printf("De %s a %s: %lu archivos, %lu kilobytes transferidos.\n", 
           "d2", "d1", files_d2_to_d1, kilobytes_d2_to_d1);
}
