# Editor de Curvas de Bézier para Juego de F1 🏎️🏁
Basicamente, es un editor de cadenas de curvas de bezier hecho para que luego estas se puedan importar a un juego de F1 que pienso hacer. Intente que sea lo mas sencillo de usar posible pero que a la vez permita ser bastante preciso. El programa cuenta con una grilla, una regla, entre otras herramientas. Fue programado con `C++` y utilizando la libreria de `Raylib`.


## 📥 Instalación
1. Descomprime el archivo `.zip`.
2. Ejecuta el archivo `.exe`.


## 🎮 Cómo Utilizarlo

### **Controles**
- **🖱️ Botón izquierdo del mouse**: Mueve los puntos para ajustar las curvas.
- **🖱️ Click izquierdo fuera de un punto**: Activa la regla.
- **🖱️ Botón derecho del mouse**: Arrastrando desde el último punto de la cadena, se genera una nueva curva.
- **🖱️ Botón de la rueda del mouse**:  
  - Haciendo click en cualquier punto de la regla, la elimina.  
  - Haciendo click en el último punto de la cadena, elimina la última curva.

- **📷 Movimiento de cámara**:
  - `WASD` para mover la cámara.
  - Rueda del mouse para hacer zoom.

- **🚧 Configuración de la pista**:
  - `↑` / `↓` (flechas) → Aumenta/disminuye el ancho de la pista en 1 metro.
  - `ALT` → Alterna entre una pista cerrada o abierta.
  - `CTRL` → Activa o desactiva la grilla.
  - `SHIFT` → Permite mover la grilla libremente (si está activada).


## 📁 Archivos
### Guardar 
Al cerrar el programa, el archivo se guarda automáticamente en la carpeta `saves` como `track_DD-MM-YY_hr-min-sec.bin`.
### Cargar 
Para cargar un archivo, colócalo en la carpeta `load` como `load_track.bin`.
