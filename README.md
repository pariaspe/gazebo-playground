# Entrega: Gazebo
Este repositorio contiene la generación de laberinto con un robot en Gazebo, moviendo el robot desde el origen hasta la meta. Este desplazamiento se consigue mediante el uso de un plugin de gazebo que ha sido desarrollado para la resolución de este ejercicio.

Autor: **Pedro Arias Pérez**

Link: [pariaspe/gazebo-playground](https://github.com/pariaspe/gazebo-playground)


## Índice
- [1. Descripción](#1-descripción)
- [2. Estructura de Carpetas](#2-estructura-de-carpetas)
- [3. Base](#3-base)
- [4. Extras](#4-extras)
    - [4.1. Extra 1](#extra-1-video)
    - [4.2. Extra 2](#extra-2-heading)
    - [4.3. Extra 3](#extra-3-control-con-pid)


---

## 1. Descripción
Para la práctica se han realizado los siguientes hitos:

- **Base**:
    1. Se presenta un **plugin básico** que actualizando las velocidades lineares consigue que el robot alcance la meta. El plugin se prueba sobre sobre un mapa sencillo (map1).

- **Extra**:
    1. Se presenta un **video** que demuestra el funcionamiento de la parte base.
    2. Control del **heading**.
    3. Control mediante **PID**.

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

```xml
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

Sobre el plugin creado mostramos los cambios realizados en el método OnUpdate.

```c++
    void OnUpdate()
    {
        ignition::math::Pose3d pose = model->WorldPose();
        printf("At: %f %f %f\n", pose.Pos().X(), pose.Pos().Y(), pose.Pos().Z());

        float dx = GOAL_X - pose.Pos().X();  // Distancia a meta
        float dy = GOAL_Y - pose.Pos().Y();

        float velx = dx / GOAL_X;  // Normalizamos
        float vely = dy / GOAL_Y;

        model->SetLinearVel(ignition::math::Vector3d(velx, vely, 0));
    }
 ```

El algoritmo decide la velocidad en función de la posición del robot y la posición de la meta. Esta velocidad se normaliza para que siempre se encuentre entre `[0-1]`.

## 4. Extras
### Extra 1: Video

Se muestra en vídeo el resultado de la ejecución de la parte base.

 [![Gazebo Plugin Base](http://img.youtube.com/vi/cw2RJPpvA7c/0.jpg)](http://www.youtube.com/watch?v=cw2RJPpvA7c)

### Extra 2: Heading

Se ha mejorado el algoritmo para que el robot esté orientado en la dirección de avance. Para ello, además de implementar un control en velocidad lineal, se añade un control en velocidad angular, en concreto en guiñada (yaw).

```c++
void OnUpdate()
{
    ignition::math::Pose3d pose = model->WorldPose();
    printf("At: %f %f %f %f %f %f\n", pose.Pos().X(), pose.Pos().Y(), pose.Pos().Z(), pose.Rot().Roll(), pose.Rot().Pitch(), pose.Rot().Yaw());

    float dx = GOAL_X - pose.Pos().X();  // Distancia a meta
    float dy = GOAL_Y - pose.Pos().Y();

    float goal_yaw = atan2(dy, dx);  // Heading a la meta

    float velx = dx / GOAL_X;  // Normalizamos
    float vely = dy / GOAL_Y;

    float v_yaw = goal_yaw - pose.Rot().Yaw();  // Corregimos el heading actual

    model->SetLinearVel(ignition::math::Vector3d(velx, vely, 0));
    model->SetAngularVel(ignition::math::Vector3d(0, 0, v_yaw));
}
```

### Extra 3: Control con PID

El control del plugin se modifica añadiendo un controlador PID que fija las velocidades en función de la posición actual del robot y de la meta.

```c++
void OnUpdate()
{
    ignition::math::Pose3d pose = model->WorldPose();
    printf("At: %f %f %f %f %f %f\n", pose.Pos().X(), pose.Pos().Y(), pose.Pos().Z(), pose.Rot().Roll(), pose.Rot().Pitch(), pose.Rot().Yaw());

    float dx = GOAL_X - pose.Pos().X();  // Distancia a meta
    float dy = GOAL_Y - pose.Pos().Y();

    float goal_yaw = atan2(dy, dx);  // Heading a la meta
    pid_yaw.set_point = goal_yaw;

    float v_yaw = pid_yaw.update(pose.Rot().Yaw()); // Actualizamos las velocidades
    float velx = pidx.update(pose.Pos().X());
    float vely = pidy.update(pose.Pos().Y());

    velx = velx / 10;  // Reducimos las velocidades obtenidas
    vely = vely / 10;

    model->SetLinearVel(ignition::math::Vector3d(velx, vely, 0));
    model->SetAngularVel(ignition::math::Vector3d(0, 0, v_yaw));
}
```

El código completo con la declaración de la clase `PID()` junto con las instanciaciones y las inicializaciones de los objetos `pidx`, `pidy` y `pid_yaw` se puede ver en el archivo `model_push.c`.
