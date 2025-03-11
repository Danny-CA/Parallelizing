# **Conteo de caracteres en paralelo con MPI**

Este proyecto explora cómo paralelizar la funcionalidad de la librería `Counter` de **Python** utilizando **MPI en C**. En lugar de contar elementos secuencialmente en Python, distribuimos la tarea entre múltiples procesos MPI, mejorando la eficiencia en textos grandes.  

## **Descripción**  
En **Python**, la clase `Counter` de `collections` permite contar elementos de manera eficiente:  
```python
from collections import Counter  
texto = "bananaappleorange"  
conteo = Counter(texto)  
print(conteo)  
# Salida: Counter({'a': 5, 'n': 3, 'p': 2, 'b': 1, 'l': 1, 'e': 2, 'o': 1, 'r': 1, 'g': 1})
```    
Sin embargo, esto no aprovecha múltiples núcleos.

## **Cómo funciona la paralelización**
**Python (Counter) es secuencial → Recorre el texto en un solo proceso.**
**MPI divide el texto en p partes (p = número de procesos).**
**Cada proceso cuenta su parte y luego usa MPI_Reduce() para combinar resultados.**
