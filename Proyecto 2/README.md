# Simulador de Teatro de Operaciones usando Hilos y Procesos

Franco Murillo - 1610782  
Andrea Carrillo - 1710107  

## En qué consiste el problema del Teatro de Operaciones
- Se tiene una cuadrícula NxM, que representa el teatro de operaciones
- Dentro de esta cuadrícula se encuentran objetos, los cuales pueden ser instituciones civiles (IC), objetos militares (OM), o tierra baldía (TB)
- Cada objeto tiene una resistencia diferente:
  - Las IC tienen resistencia > 0
  - Los OM tienen resistencia < 0
  - Las TB tienen resistencia 0 
- La idea del teatro es desplegar y detonar cierta cantidad de drones a lo largo de la cuadrícula
- Estos drones tienen un poder explosivo y un rango de explosión, en el cual podría entrar un objeto
- Una instancia de un teatro de operaciones es así:
  - Número de filas y columnas del teatro (N y M)
  - Número de objetos, y posteriormente los objetos (solamente de tipo IC u OM, los TB no son relevantes; se indica sus coordenadas en la cuadrícula, y su resistencia)
  - Número de drones, y posteriormente los drones (indicando sus coordenadas en la cuadrícula, su rango de explosión y su poder explosivo)
- Un ejemplo de instancia es como sigue (esta instancia es la que se encuentra como prueba en las carpetas con las soluciones):
```
10 10
4
0 0 8
5 5 100
1 1 -2
7 7 -6
5
2 1 2 3
1 1 1 4
7 7 0 3
6 6 4 8
9 9 8 1
```

## Cuál es el objetivo del proyecto
- El objetivo del proyecto es desarrollar un programa que, dada una instancia de un teatro de operaciones, verifique si los objetos OM e IC entran en los rangos explosivos de los drones, y determine cuántos son destruidos parcial o totalmente; también debe indicar cuántos objetos no fueron afectados por ninguna explosión
- Para el ejemplo de instancia anterior, el resultado se vería así:
```
OM sin destruir: 0
OM parcialmente destruidos: 0
OM totalmente destruido: 2
IC sin destruir: 0
IC parcialmente destruidos: 2
IC totalmente destruido: 0
```
- Además, se quiere desarrollar de forma que sea más eficiente con respecto a usar un único proceso para resolverlo
- Para esto, se desarrollaron dos soluciones, una con **Hilos**, y otra con **Procesos**
- El proyecto fue realizado en C
