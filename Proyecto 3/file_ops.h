// Archivo que contendra las definiciones de las operaciones para sincronizar los dos directorios
#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
#include <limits.h>

// Funcion que copia un archivo regular (src_file) al directorio (dest_dir).
int cp_file_to_dir(const char *src_file, const char *dest_dir);

// Funcion que copia recursivamente un directorio (src_dir) al directorio destino (dest_dir).
int cp_dir_to_dir(const char *src_dir, const char *dest_dir);

// Funcion que borra recursivamente el contenido de un directorio.
int rm_dir(const char *dir_path);

/*
  Funcion que compara dos archivos (file1 y file2) y retorna true (1) si tienen el mismo contenido,
  o false (0) en caso contrario.
 */
int same_content_file(const char *file1, const char *file2);

// Funcion que sincroniza el contenido de dos directorios (la funcion fue tan larga que se implemento en otro archivo)
void sync_dirs(const char *d1, const char *d2);

/*
 * Funcion que muestra el reporte de estadisticas de transferencia (cantidad de archivos y kilobytes
 * copiados en cada sentido) por salida estandar.
 */
void print_transfer_statistics(void);

#endif  // FILE_OPS_H
