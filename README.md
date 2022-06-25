# vida
Juego de la Vida de Conway

![screenshot1](https://user-images.githubusercontent.com/75378876/175790480-24f686a2-096e-47ef-9fd2-eade38902569.png)

![screenshot2](https://user-images.githubusercontent.com/75378876/175790482-98d2c7d0-b889-4c7a-8fa7-069439969d65.png)

El Juego de la Vida de Conway es un autómata celular muy sencillo y popular creado por John Horton Conway en 1970
El Juego consiste en un tablero con forma de cuadrícula que contiene una célula por cuadro, aunque en la versión original el tablero es infinito, mi implementación es con bordes, es decir, el universo es finito.
Las células tiene dos estados, pueden estar vivas o muertas.
El tablero evoluciona a lo largo de unidades de tiempo discretas. El estado de todas las células se tiene en cuenta para calcular el estado de las mismas al turno siguiente.
Las transiciones dependen del número de células vecinas vivas:
Una célula muerta con exactamente 3 células vecinas vivas “nace”
Una célula viva con 2 ó 3 células vecinas vivas sigue viva, en otro caso muere por "soledad" o "superpoblación"
