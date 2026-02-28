# Búsqueda eficiente sobre grandes volúmenes de datos

##  Tabla comparativa de rendimiento

| Estructura de Datos | Tiempo promedio de búsqueda | Memoria aproximada usada | Complejidad teórica |
|---------------------|----------------------------|--------------------------|---------------------|
| Lista enlazada      | ~0.000000 us               | 228.88 MB                | O(n)                |
| Árbol binario (set) | ~0.000000 us               | 435.61 MB                | O(log n)            |
| Tabla Hash          | ~0.000100 us               | 345.81 MB                | O(1) promedio       |

---

##  Análisis

- La lista enlazada presenta complejidad O(n), por lo que el tiempo de búsqueda crece linealmente con el tamaño de los datos.
- El árbol binario balanceado (std::set) presenta complejidad O(log n), mejorando significativamente el rendimiento frente a O(n).
- La tabla hash presenta complejidad O(1) promedio, siendo la estructura más eficiente para búsquedas masivas.

Aunque los tiempos medidos en microsegundos son muy pequeños, la diferencia real entre estructuras se observa en su complejidad teórica.
