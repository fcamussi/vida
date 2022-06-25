# Vida

Vida es una implementación del juego de la vida de Conway, un autómata celular muy sencillo y popular creado por John Horton Conway en 1970.

El Juego consiste en un tablero con forma de cuadrícula que contiene una célula por cuadro, aunque en la versión original el tablero es infinito, mi implementación es con bordes, es decir, el universo es finito.

Las células tiene dos estados, pueden estar vivas o muertas. El tablero evoluciona a lo largo de unidades de tiempo discretas. El estado de todas las células se tiene en cuenta para calcular el estado de las mismas al turno siguiente.

Las transiciones dependen del número de células vecinas vivas:
* Una célula muerta con exactamente 3 células vecinas vivas "nace"
* Una célula viva con 2 ó 3 células vecinas vivas sigue viva, en otro caso muere por "soledad" o "superpoblación"

## Screenshots

![screenshot1](https://user-images.githubusercontent.com/75378876/175793408-4b61c119-5cb7-473d-be93-5f15c4054afa.png)

![screenshot2](https://user-images.githubusercontent.com/75378876/175793410-6e61213d-bcbe-4d1c-a47b-3133872d8d56.png)

## Requisitos

* gcc
* GTK+ 2

## Compilación

```
make
```

## Ejecución

```
./vida
```
