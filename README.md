# Entrega: Gazebo
Este repositorio contiene la generación de laberinto con un robot en Gazebo, moviendo el robot desde el origen hasta la meta. Este desplazamiento se consigue mediante el uso de un plugin de gazebo que ha sido desarrollado para la resolución de este ejercicio.

Autor: **Pedro Arias Pérez**

Link: [pariaspe/gazebo-playground](https://github.com/pariaspe/gazebo-playground)


## Índice
- [1. Descripción](#1-descripción)
- [2. Estructura de Carpetas](#2-estructura-de-carpetas)
- [3. Base](#3-base)
- [4. Extras](#4-extras)
    - [4.1. Extra 1](#extra-1-prueba)

---

## 1. Descripción
Para la práctica se han realizado los siguientes hitos:

- **Base**:
    1. Lorem ipsum

- **Extra**:
    1. Lorem ipsum
    
## 2. Estructura de carpetas
El esquema de organización del reposition es el siguiente:
```
*dirs*
```

## 3. Base
Tras finalizar la instalación se calculan las dimensiones del mapa:

```bash
parias@parias-msi:~/repos/gazebo-tools$ python3
Python 3.6.9 (default, Oct  8 2020, 12:12:24) 
[GCC 8.4.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> x = len("arias")*2
>>> y = len("perez")*2
>>> print(x, y)
10 10
>>> 
```

Dadas estas dimensiones `(10x10)` se ha construído el siguiente mapa en formato csv `(map1.csv)`:

```bash
parias@parias-msi:~/repos/gazebo-tools$ cat assets/map1.csv 
1,1,1,1,1,1,1,1,1,1
1,0,0,0,0,0,0,0,0,1
1,0,0,0,0,0,0,0,0,1
1,0,0,0,0,0,0,0,0,1
1,1,1,1,0,0,0,0,0,1
1,1,1,1,0,0,0,0,0,1
1,0,0,0,0,0,0,0,0,1
1,0,0,0,0,0,0,0,0,1
1,0,0,0,0,0,0,0,0,1
1,1,1,1,1,1,1,1,1,1
```

Se ha elegido un mapa muy sencillo con una resolución de un metro por caracter. El inicio será la posición `[1,1]` (fila 2, columna 2), mientras que la meta será en la posición `[8,8]` (fila 9, columna 9).

Utilizando `gazebo-map-from-csv.py` se genera el mundo de gazebo:

```bash
parias@parias-msi:~/repos/gazebo-tools$ python gazebo-map-from-csv.py 
[[1. 1. 1. 1. 1. 1. 1. 1. 1. 1.]
 [1. 0. 0. 0. 0. 0. 0. 0. 0. 1.]
 [1. 0. 0. 0. 0. 0. 0. 0. 0. 1.]
 [1. 0. 0. 0. 0. 0. 0. 0. 0. 1.]
 [1. 1. 1. 1. 0. 0. 0. 0. 0. 1.]
 [1. 1. 1. 1. 0. 0. 0. 0. 0. 1.]
 [1. 0. 0. 0. 0. 0. 0. 0. 0. 1.]
 [1. 0. 0. 0. 0. 0. 0. 0. 0. 1.]
 [1. 0. 0. 0. 0. 0. 0. 0. 0. 1.]
 [1. 1. 1. 1. 1. 1. 1. 1. 1. 1.]]
lines = X = 10
columns = Y = 10
```

![map.world.xml](/doc/map1.png)

Se añade el Pioneer en la posición `[1.45, 1.6]` modificando a mano el archivo del mundo. Además, incluímos el plugin (`model_push`) creado para desplazar el robot hasta la meta.

```
<sdf version="1.4">
  <world name="default">
    <light name="sun" type="directional">
      <cast_shadows>1</cast_shadows>
      <diffuse>0.8 0.8 0.8 1</diffuse>
      <specular>0.1 0.1 0.1 1</specular>
      <attenuation>
        <range>1000</range>
        <constant>0.9</constant>
        <linear>0.01</linear>
        <quadratic>0.001</quadratic>
      </attenuation>
      <direction>-0.5 0.5 -1</direction>
    </light>
    <include>
      <uri>model://pioneer</uri>
      <pose>1.45 1.6 0 0 0 0</pose>
      <plugin name="model_push" filename="libmodel_push.so"/>
    </include>
    <model name="floor">
      <pose>5.0 5.0 -0.0625 0 0 0</pose>
      <static>true</static>
          ...
    </model>
      ...
  </world>
</sdf>
```

![pioneer](/doc/pioneer.png)

El plugin creado es el siguiente:

```
*code*
```

Finalmente se muestra en este vídeo el resultado obtenido.

![video](/link/to/video)

## 4. Extras
### Extra 1: Prueba
Lorem ipsum
