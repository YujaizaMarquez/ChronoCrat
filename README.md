# ChronoCrat

## Requisitos

- CMake 3.24 o superior.
- Un compilador de C++17 (por ejemplo, MinGW en Windows o GCC/Clang en Linux).
- Git y conexión a Internet durante la primera configuración para descargar SFML
  y DESIGNAR.
- En Linux, las bibliotecas de desarrollo de X11 y OpenGL que requiere SFML.

## Compilar y ejecutar

Desde la raíz del proyecto, compila y ejecuta con GNU Make:

```sh
make run
```

En la primera ejecución, CMake descarga SFML y DESIGNAR automáticamente a la
carpeta `build`; no es necesario instalar ni copiar esas bibliotecas al repositorio.

En Windows, el ejecutable se genera en `build/bin/MiProyectoSFML.exe`; en Linux,
en `build/bin/MiProyectoSFML`. CMake copia la carpeta `assets` junto al ejecutable.

Los parámetros ajustables del juego (tamaños, colisiones, armas, enemigos,
oleadas y HUD) están centralizados por secciones en
[`include/GameConfig.hpp`](include/GameConfig.hpp). En el Nivel 1, el fondo,
los límites de las tres arenas, las cantidades de soldados y los obstáculos
se configuran en `GameConfig::Level1`. Los datos presentes en los archivos JSON
siguen teniendo prioridad para las entidades que los cargan.