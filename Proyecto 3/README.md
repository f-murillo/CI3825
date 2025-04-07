# Sincronizando Directorios con C

Franco Murillo - 1610782  

Andrea Carrillo - 1710107

----------------------------------------------------------------------------------------------------------------------------

- El objetivo es __sincronizar dos directorios__, ya sea copiando archivos o subdirectorios desde un directorio a otro, o borrándolos.

- Al final de la sincronización, ambos directorios deben tener __los mismos elementos__.

----------------------------------------------------------------------------------------------------------------------------

## Sobre el Proyecto

- El proyecto fue realizando en __C__ (como dice el título).

- OBSERVACION: el proyecto está hecho para sistemas basados en __UNIX__ (Linux).

- La idea era hacerlo sin usar librerías ni funciones externas a la líbrería estándar de C con sus extensiones GNU.

- Para esto, se implementaron las siguientes funciones, haciendo uso de la librería estándar con extensiones GNU, y con el uso de __llamadas al sistema__:

    - Una función que __copia un archivo de un directorio a otro__ (cp_file_to_dir).

    - Una función que __copia un subdirectorio desde un directorio a otro__ recursivamente (cp_dir_to_dir).

    - Una función que __borra un subdirectorio__ recursivamente (rm_dir).

    - Una función que __verifica si dos archivos tienen el mismo contenido__ (same_content_file).

    - Una función que __sincroniza dos directorios__, usando las funciones anteriores (sync_dirs).

- Al ejecutar el método principal, se deben pasar las direcciones de los dos directorios a sincronizar.

    - Si se encuentra algún archivo o subdirectorio que se encuentre en uno de los directorios pero no en el otro, se preguntará si se desea copiarlo, o borrarlo.

    - Si se encuentra algún archivo del mismo nombre en ambos directorios, habrá dos casos:

        - Si tienen el mismo contenido, no se hace nada.

        - Si no tienen el mismo contenido, el programa recomendará conservar el archivo más reciente en ambos directorios, pero la decisión quedará en manos del usuario.

    - Si se encuentra algún subdirectorio del mismo nombre en ambos directorios, se sincronizan de manera recursiva. 

----------------------------------------------------------------------------------------------------------------------------

## Compilación y Ejecución

### Creación del ejecutable

```
make 
```

### Creación del ejecutable detectando errores en el código antes de su ejecución.

```
scan-build make
```

### Ejecución del programa

```
./syncDir <camino al directorio 1> <camino al directorio 2>
```

### Ejecución del programa con Valgrind (para comprobar gestión de la memoria y posibles errores)

```
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  ./syncDir <camino al directorio 1> <camino al directorio 2>
```

### Ejemplo de Uso

- Sean los directorios "USB" y "Docs", que se encuentran en el mismo directorio que el ejecutable __syncDir__.

- USB contiene los siguientes elementos:

    - Un directorio "ci3825", con dos archivos "Ex1.txt" y "Lab1.txt" vacíos.

    - Un directorio "ci2611", con un archivo "Proy1.pdf".

    - Un archivo "TODO" que dice "Versión nueva de TODO".

- Docs contiene los siguientes elementos:

    - Un directorio "backup", que contiene lo siguiente:
        
        - Un directorio __"OLDUSB"__, el cual contiene:

            - Un directorio "ci3825", que contiene un archivo "Lab1.txt" vacío.

            - Un directorio "ci5437", que contiene el archivo "IA1.txt" vacío.

            - Un archivo "TODO" que dice "Versión antigua de TODO".

- Se quieren sincronizar los directorios __USB__ y __OLDUSB__. La ejecución del programa es como sigue:

```
./syncDir USB Docs/backup/OLDUSB
El elemento 'ci2611' existe en USB pero no en Docs/backup/OLDUSB.
Desea copiarlo a Docs/backup/OLDUSB (c) o borrarlo de USB (b)? c
Directorio 'ci2611' copiado de USB a Docs/backup/OLDUSB.
El elemento 'Ex1.txt' existe en USB/ci3825 pero no en Docs/backup/OLDUSB/ci3825.
Desea copiarlo a Docs/backup/OLDUSB/ci3825 (c) o borrarlo de USB/ci3825 (b)? c
Archivo 'Ex1.txt' copiado de USB/ci3825 a Docs/backup/OLDUSB/ci3825.

El archivo 'TODO' difiere en USB y Docs/backup/OLDUSB.
Fecha de modificacion en USB: 2025-04-07 17:15:03
Fecha de modificacion en Docs/backup/OLDUSB: 2025-03-28 17:21:22
Se recomienda copiar el archivo con fecha mas reciente.
Cual version desea mantener? (1: version en USB, 2: version en Docs/backup/OLDUSB): 2
Archivo 'TODO' actualizado en USB.
El elemento 'ci5437' existe en Docs/backup/OLDUSB pero no en USB.
Desea copiarlo a USB (c) o borrarlo de Docs/backup/OLDUSB (b)? c
Directorio 'ci5437' copiado de Docs/backup/OLDUSB a USB.

Estadisticas de Transferencia:
De d1 a d2: 2 archivos, 0 kilobytes transferidos.
De d2 a d1: 2 archivos, 0 kilobytes transferidos.
```

- OBSERVACION: en las estadísticas de transferencia, aparecen 0 kilobytes transferidos, porque todos los archivos están vacíos.
