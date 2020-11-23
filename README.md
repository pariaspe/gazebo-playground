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
    - [4.4. Extra 4](#extra-4-nuevo-mapa)
    - [4.4. Extra 5](#extra-5-vídeo-extra)

---

## 1. Descripción
Para la práctica se han realizado los siguientes hitos:

- **Base**:
    1. Se presenta un **plugin básico** que actualizando las velocidades lineares consigue que el robot alcance la meta. El plugin se prueba sobre sobre un mapa sencillo (map1).

- **Extra**:
    1. Se presenta un **video** que demuestra el funcionamiento de la parte base.
    2. Control del **heading**.
    3. Control mediante **PID**.
    4. Nuevo **mapa**. El mapa base se ha modificado con nuevos modelos.
    5. **Vídeo** de la parte **extra**.

## 2. Estructura de carpetas
El esquema de organización del repositorio es el siguiente:
```
.
+-- doc (imgs...)
+-- model_push
    +-- CMakeLists.txt
    +-- model_push.cc
    +-- model_push.world
    +-- README.md
+-- tools
    +-- assets
        +-- map1.csv
    +-- models
        +-- construction_cone (materials, meshes, sdfs..)
        +-- grass_plane (materials, config, sdf)
        +-- iris_fool (sdf, config)
        +-- logo (materials, config, sdf)
    +-- gazebo-map-from-csv.py
    +-- map.world.xml
+-- README.md
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

### Extra 4: Nuevo mapa

El mapa se ha modificado incluyendo nuevos modelos y modificando alguno ya existente. El nuevo mundo se puede ver en la siguiente imagen.

![mundo](/doc/mundo-extra.png)

El mapa se ha modificado editando el fichero `map.world.xml`. En el fichero se han incluido un nuevo suelo (sustituyendo al anterior), un logo de la UC3M, tres conos de construcción y un drone iris, todos como modelos sdf. Además, las cajas `box_0_0` y `box_9_9` han sido editadas añadiendoles un material que les da color.

Sobre los modelos es importante destacar:

- El suelo (`grass_plane`) y los conos (`construction_cone`) se han cogido de la librería de modelos de gazebo y no han sido modificados [[1]](https://github.com/osrf/gazebo_models).
- El drone iris (`iris_fool`) se ha cogido de la librería de modelos de PX4 y ha sido modificado comentando los plugins de control del drone que no son necesarios en este ejemplo [[2]](https://github.com/PX4/PX4-SITL_gazebo/tree/cd8ba25c81f32d6fe088482e37f8a38892209ef4/models).
- El modelo del logo (`logo`) ha sido creado integramente por mí y la imagen usada como textura en el material se ha obtenido de la página oficial de la UC3M bajo licencia CC [[3]](http://ocw.uc3m.es/ingenieria-informatica/ingenieria-de-la-informacion/imagenes/logo_uc3m.jpg/view).

Por otro lado los materiales utilizados para colorear ambas cajas se han cogido de los materiales estándar de Gazebo [[4]](http://wiki.ros.org/simulator_gazebo/Tutorials/ListOfMaterials).

Por tanto, para una correcta ejecución del código es necesario copiar los modelos `iris_fool` y `logo` al directorio `~/.gazebo/models` o incluir la ruta de los modelos a la variable de entorno `GAZEBO_MODEL_PATH`. Además de estos dos modelos, también se incluyen `grass_plane` y `construction_cone` al directorio `tools/models` de este repositorio.

El mundo resultante con estos modelos es el siguiente:

```xml
<sdf version="1.4">
  <world name="default">

	<!-- Light Sun -->
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

	<!-- Pioneer -->
	<include>
	  <uri>model://pioneer</uri>
	  <pose>1.45 1.6 0 0 0 0</pose>
      <plugin name="model_push" filename="libmodel_push.so"/>
	</include>

	<!-- Floor -->
	<include>
		<uri>model://grass_plane</uri>
	</include>

	<!-- uc3m logo -->
	<include>
		<name>logo</name>
		<uri>model://logo</uri>
		<pose>5 3 1 0 0 1.5708</pose>
	</include>

	<!-- cones -->
	<include>
		<name>cone1</name>
		<uri>model://construction_cone</uri>
		<pose>0.5 9.5 1 0 0 0</pose>
	</include>
	<include>
		<name>cone2</name>
		<uri>model://construction_cone</uri>
		<pose>7.5 2.5 0 0 0 0</pose>
	</include>
	<include>
		<name>cone3</name>
		<uri>model://construction_cone</uri>
		<pose>5.5 5.5 0 0 0 0</pose>
	</include>

	<!-- Iris drone -->
	<include>
		<name>drone</name>
		<uri>model://iris_fool</uri>
		<pose>9.5 0.5 1.075 0 0 0</pose>
	</include>

    <!-- Boxes -->
    <model name="box_0_0">
      <pose>0.5 0.5 0.5 0 0 0</pose>
      <static>true</static>
      <link name="link">
        <collision name="collision">
          <geometry>
            <box>
              <size>1.0 1.0 1.0</size>
            </box>
          </geometry>
        </collision>
        <visual name="visual">
          <geometry>
            <box>
              <size>1.0 1.0 1.0</size>
            </box>
          </geometry>

		  <material>
		    <script>
		      <uri>file://media/materials/scripts/gazebo.material</uri>
		      <name>Gazebo/Red</name>
		    </script>
		  </material>
        </visual>
      </link>
    </model>

    ...

  </world>
</sdf>
```

### Extra 5: Vídeo extra

Se muestra en vídeo el resultado de la ejecución con todos los extras realizados. En el se puede observar el nuevo mundo, el control del heading y el uso de PID para el control de las velocidades del robot. Además, en el video se puede observar que pese a colisionar con un objeto móvil como el cono el robot llega a su destino.

 [![Gazebo Plugin Base](http://img.youtube.com/vi/pki9Rzygo4M/0.jpg)](http://www.youtube.com/watch?v=pki9Rzygo4M)
