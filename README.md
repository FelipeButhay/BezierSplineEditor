# Editor de Curvas de Bezier para juego de F1
Basicamente, es un editor de cadenas de curvas de bezier hecho para que luego estas se puedan importar a un juego de F1 que pienso hacer. Intente que sea lo mas sencillo de usar posible pero que a la vez permita ser bastante preciso. El programa cuenta con una grilla, una regla, entre otras herramientas.

## Instalacion
Simplemente se descomprime el zip y se ejecuta el archivo '.exe'.

## Como Utilizarlo
### Controles

* Con el boton izquierdo del mouse se pueden mover los distintos puntos para asi ajustar las curvas.
* Si se hace click izquierdo fuera de un punto se activa la regla.
* Con el boton derecho del mouse, arrastrando desde el ultimo punto de la cadena, se puede generar una nueva curva.
* Con el boton de la rueda del mouse se puede eliminar tanto la regla (haciendo click en cualquiera de los puntos) como la ultima curva (haciendo click en el ulitmo punto de la cadena).

* Con las teclas 'WASD' se puede mover la camara y con la rueda del mouse ajustar el zoom.
* Con las flechas de 'UP' y 'DOWN' se incrementa o decrementa en un metro el ancho de la pista respectivamente.
* Con la tecla 'ALT' se alterna entre una pista cerrada o abierta.
* Con la tecla 'CTRL' se alterna prende o apaga la grilla. Si esta la grilla activada se puede mover libremente manteniendo 'SHIFT'

### Archivos

* Para guardar un archivo simplemente hay que cerrar el programa y el archivo se guardara automaticamente en la carpeta 'saves' en el siguiente formato 'track_DD-MM-YY_hr-mim-sec.bin'
* Para cargar un archivo simplemente hay que colocarlo en la carpeta 'load' con el nombre 'load_track.bin'.
