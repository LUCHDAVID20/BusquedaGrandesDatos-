Búsqueda eficiente sobre grandes volúmenes de datos

Este programa genera 10,000,000 de números aleatorios en el rango [-50,000,000, 50,000,000], los almacena en un archivo de texto y posteriormente los carga en memoria para comparar el rendimiento de búsqueda utilizando distintas estructuras de datos.

El objetivo principal es analizar la **complejidad teórica** y el comportamiento práctico de cada estructura.

Arreglo ordenado (vector + binary_search)

- Los datos se ordenan previamente.
- Se utiliza búsqueda binaria.
- Complejidad de búsqueda: **O(log n)**.
- Uso de memoria: Bajo en comparación con la tabla hash.

 Tabla Hash (unordered_set)

- Acceso directo mediante función hash.
- No requiere ordenamiento previo.
- Complejidad promedio de búsqueda: **O(1)**.
- Uso de memoria: Alto debido al almacenamiento interno de buckets.

orden

1. Generación de 10 millones de números aleatorios.
2. Almacenamiento en archivo de texto.
3. Carga de datos en memoria.
4. Construcción de ambas estructuras.
5. Ejecución de 1,000 búsquedas automáticas.
6. Medición del tiempo promedio por estructura.
7. Presentación de tabla comparativa.

---

## 📊 Resultados obtenidos (ejemplo)

| Estructura | Tiempo promedio | Memoria aproximada | Complejidad |
|------------|-----------------|--------------------|-------------|
| Arreglo ordenado | Muy bajo | ~38 MB | O(log n) |
| Tabla Hash | Muy bajo | ~345 MB | O(1) promedio |



Análisis de complejidad teórica

- **O(1)**: Tiempo constante. No depende del tamaño de los datos.
- **O(log n)**: Crecimiento lento. Aumenta ligeramente cuando n crece.
- En grandes volúmenes de datos, O(1) es más eficiente que O(log n).

 Conclusión

La tabla hash es más eficiente en tiempo de búsqueda debido a su complejidad O(1) promedio. Sin embargo, consume significativamente más memoria.

El arreglo ordenado ofrece un buen equilibrio entre eficiencia y uso de memoria, aunque su búsqueda es ligeramente más lenta debido a su complejidad O(log n).

La elección de la estructura depende del escenario y los recursos disponibles.

Lenguaje

- C++
- Visual Studio 2022
- STL (vector, unordered_set, algorithm, chrono)

---
