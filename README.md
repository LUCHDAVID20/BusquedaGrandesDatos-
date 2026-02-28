# Comparación de estructuras de datos en grandes volúmenes

Descripción

Este programa genera 10,000,000 de números aleatorios en el rango [-50,000,000, 50,000,000] y compara el rendimiento de búsqueda utilizando tres estructuras de datos diferentes.

El objetivo principal es analizar y comprender la **complejidad teórica** de cada estructura.

Estructuras implementadas

 Lista enlazada (`std::list`)

- La búsqueda se realiza recorriendo elemento por elemento.
- Complejidad de búsqueda: **O(n)**.
- A medida que aumenta la cantidad de datos, el tiempo crece de forma lineal.

---

 Árbol binario balanceado (`std::set`)

- Implementado mediante un árbol rojo-negro.
- Mantiene los datos ordenados automáticamente.
- Complejidad de búsqueda: **O(log n)**.
- El crecimiento del tiempo es mucho más lento que en O(n).

---

 Tabla hash (`std::unordered_set`)

- Utiliza una función hash para acceso directo.
- No mantiene los datos ordenados.
- Complejidad promedio de búsqueda: **O(1)**.
- Es la más eficiente en tiempo de búsqueda en promedio.

---

Resultados que obtuve
| Estructura | Complejidad de Búsqueda |
|------------|------------------------|
| Lista enlazada | **O(n)** |
| Árbol binario balanceado | **O(log n)** |
| Tabla hash | **O(1) promedio** |

---

Mi Análisis

- La lista enlazada es la menos eficiente para búsquedas grandes debido a su complejidad lineal.
- El árbol binario mejora considerablemente el rendimiento al utilizar división logarítmica.
- La tabla hash es la más rápida en promedio, ya que su tiempo no depende directamente del tamaño de los datos.

En grandes volúmenes de información, la diferencia entre O(n), O(log n) y O(1) es significativa.

