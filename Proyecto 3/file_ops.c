#include "file_ops.h"

// Funcion que copia un archivo regular (src_file) al directorio (dest_dir).
int cp_file_to_dir(const char *src_file, const char *dest_dir){
    char dest_file[PATH_MAX];
    char src_copy[PATH_MAX];

    // Hacemos una copia de src_file, ya que basename() puede modificar su cadena de entrada 
    strncpy(src_copy, src_file, PATH_MAX);
    src_copy[PATH_MAX - 1] = '\0';

    char *base = basename(src_copy);
    snprintf(dest_file, sizeof(dest_file), "%s/%s", dest_dir, base);

    int fd_src = open(src_file, O_RDONLY);
    if(fd_src < 0){
        perror("cp_file_to_dir: error abriendo archivo origen");
        return -1;
    }

    int fd_dest = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd_dest < 0){
        perror("cp_file_to_dir: error creando archivo destino");
        close(fd_src);
        return -1;
    }

    char buffer[4096];
    ssize_t bytes_read, bytes_written;
    while ((bytes_read = read(fd_src, buffer, sizeof(buffer))) > 0){
        bytes_written = write(fd_dest, buffer, bytes_read);
        if(bytes_written != bytes_read){
            perror("cp_file_to_dir: error escribiendo en archivo destino");
            close(fd_src);
            close(fd_dest);
            return -1;
        }
    }
    if(bytes_read < 0){
        perror("cp_file_to_dir: error leyendo archivo origen");
    }

    close(fd_src);
    close(fd_dest);
    return 0;
}

// Funcion que copia recursivamente un directorio (src_dir) al directorio destino (dest_dir).
int cp_dir_to_dir(const char *src_dir, const char *dest_dir){
    char new_dest[PATH_MAX];
    char src_copy[PATH_MAX];

    // Usamos basename para obtener el nombre del directorio origen 
    strncpy(src_copy, src_dir, PATH_MAX);
    src_copy[PATH_MAX - 1] = '\0';
    char *base = basename(src_copy);

    snprintf(new_dest, sizeof(new_dest), "%s/%s", dest_dir, base);

    if(mkdir(new_dest, 0755) < 0){
        if(errno != EEXIST){
            perror("cp_dir_to_dir: error creando directorio destino");
            return -1;
        }
    }

    DIR *dir = opendir(src_dir);
    if(!dir){
        perror("cp_dir_to_dir: error abriendo directorio origen");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL){
        // Omitimos entradas especiales, como "." y ".." 
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char src_path[PATH_MAX];
        snprintf(src_path, sizeof(src_path), "%s/%s", src_dir, entry->d_name);

        // Para la copia recursiva, la ruta destino se arma a partir de new_dest 
        struct stat st;
        if(stat(src_path, &st) < 0){
            perror("cp_dir_to_dir: error en stat");
            continue;
        }

        if(S_ISDIR(st.st_mode)){
            cp_dir_to_dir(src_path, new_dest);
        } else if(S_ISREG(st.st_mode)){
            cp_file_to_dir(src_path, new_dest);
        }
    }
    closedir(dir);
    return 0;
}

// Funcion que borra recursivamente el contenido de un directorio.
int rm_dir(const char *dir_path){
    DIR *dir = opendir(dir_path);
    if(!dir){
        perror("rm_dir: error abriendo directorio");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL){
        // Omitimos entradas especiales, como "." y ".." 
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if(stat(path, &st) < 0){
            perror("rm_dir: error en stat");
            continue;
        }

        if(S_ISDIR(st.st_mode)){
            // Llamada recursiva para eliminar subdirectorios 
            rm_dir(path);
            if(rmdir(path) < 0){
                perror("rm_dir: error eliminando directorio");
            }
        } else if(S_ISREG(st.st_mode)){
            if(remove(path) < 0){
                perror("rm_dir: error eliminando archivo");
            }
        }
    }
    closedir(dir);
    return 0;
}

/*
  Funcion que compara dos archivos (file1 y file2) y retorna true (1) si tienen el mismo contenido,
  o false (0) en caso contrario.
 */
int same_content_file(const char *file1, const char *file2){
    struct stat st1, st2;
    if(stat(file1, &st1) < 0 || stat(file2, &st2) < 0){
        perror("same_content_file: error en stat");
        return 0;
    }

    if(st1.st_size != st2.st_size)
        return 0;

    int fd1 = open(file1, O_RDONLY);
    int fd2 = open(file2, O_RDONLY);
    if(fd1 < 0 || fd2 < 0){
        perror("same_content_file: error abriendo archivos");
        if(fd1 >= 0) close(fd1);
        if(fd2 >= 0) close(fd2);
        return 0;
    }

    char buf1[4096], buf2[4096];
    ssize_t n1, n2;
    int iguales = 1;
    while ((n1 = read(fd1, buf1, sizeof(buf1))) > 0 &&
           (n2 = read(fd2, buf2, sizeof(buf2))) > 0){
        if(n1 != n2 || memcmp(buf1, buf2, n1) != 0){
            iguales = 0;
            break;
        }
    }

    close(fd1);
    close(fd2);
    return iguales;
}
