Observaciones 

- Se usaron apuntadores a la estructura nodo en lugar de usar la estructura directamente porque de esta manera se pueden crear los nodos de manera dinámica, lo cual es necesario para el sistema de archivos. Además, usar la estructura directamente implica que al pasar la estructura a una función, se crea una copia de toda la estructura, lo cual termina siendo ineficiente. Se encontraron comentarios al respecto en https://stackoverflow.com/questions/61600147/why-use-pointer-to-struct-and-not-use-the-struct-directly-c y en https://www.reddit.com/r/C_Programming/comments/8i6r79/is_there_a_reason_to_use_a_pointer_to_a_struct/?rdt=57648

- No fue posible definir los métodos y funciones para la estructura en node.c teniendo la definición formal en node.h. Se encontró una explicación y una solución en https://stackoverflow.com/questions/36273873/typedef-c-struct-invalid-use-of-incomplete-typedef . Se decidió usar funciones de acceso para los campos de la estructura, manteniendo la definición de la estructura intacta en node.h.

- En la función deleteNode:
    - Se libera la memoria del nombre del nodo porque le fue asignada memoria en el heap por la función strdup().
    - No es necesario liberar la memoria del resto de campos del nodo (como padre, hijo, hermano) porque son apuntadores a otros nodos, no se les asigna memoria.
    - Al campo de la fecha de creación se le asigna memoria, pero en la pila, por lo que no necesita ser liberada de memoria 

- Se vio que el comportamiento de los métodos touch y mkdir eran muy parecidos, repitiendo muchas secciones de código, se decidió crear una función auxiliar addElement que contiene todo el comportamiento similar entre rm y mkdir. Lo mismo ocurre con los métodos rm y rmdir, por lo que de igual forma se implementó una función auxiliar removeElement.

- Por otra parte, se vió que las funciones addElement y removeElement relizaban la misma verificación del camino proporcionado, por lo que se decidió también implementar una función auxiliar verifyPath que contiene dicha verificación.

- Sobre la función verifyPath:
    - Se hace uso de dobles punteros para el nombre del elemento a crear y el directorio destino, ya que permite que la función verifyPath cambie directamente el valor de dicho nombre y directorio en el método desde la cual es llamada. 

- Se estableció como límite de caracteres para los caminos absolutos a 256 caracteres, porque se cree que son suficientes para almacenar caminos absolutos largos


------------------------------------------------------------------------------------------------------------------------------------------------

Creación del ejecutable

make 

Creación del ejecutable detectando errores en el código antes de su ejecución.

scan-build make

Ejecución del programa

./simfs

Ejecución  del programa leyendo comandos desde un archivo, agrega el nombre del archivo como argumento

./simfs nombre_del_archivo

Ejecución del programa con Valgrind para comprobar gestión de la memoria y posibles errores

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  ./simfs

Ejecución  del programa con Valgrind leyendo comandos desde un archivo

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  ./simfs nombre_del_archivo

Ejemplo de uso

```
> touch archivo_p
Archivo 'archivo_p' creado correctamente.
> mkdir dir_p
Directorio 'dir_p' creado correctamente.
> cd dir_p
Ahora estas en 'dir_p'
> mkdir hola
Directorio 'hola' creado correctamente.
> cd hola
Ahora estas en 'hola'
> cd ..
Ahora estas en 'dir_p'
> cd ..
Ahora estas en '/'
> ls     
archivo_p  dir_p  
> ls -l
archivo_p 12:12-12/02/25 F
dir_p 12:12-12/02/25 D

> rm archivo_p
Archivo 'archivo_p' eliminado.
> ls -l
dir_p 12:12-12/02/25 D

> rmdir dir_p
Error: El directorio 'dir_p' no está vacío.
> exit
Saliendo del programa
```
