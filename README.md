# **Paralelizando `Counter` de Python con MPI en C**  

This is the final project of the subject cómputo de alto desempeño of the [<ins>Escuela Nacional de Estudios Superiores UNAM campus Morelia </ins>](https://www.enesmorelia.unam.mx).

---

## License
This project is licensed under the MIT License. Please refer to the LICENSE document for more details regarding referencing and usage of this project.

---

## **Introduction**  
En **Python**, la clase `Counter` de `collections` permite contar elementos de manera eficiente. 
```python
from collections import Counter  
texto = "bananaappleorange"  
conteo = Counter(texto)  
print(conteo)  
# Salida: Counter({'a': 5, 'n': 3, 'p': 2, 'b': 1, 'l': 1, 'e': 2, 'o': 1, 'r': 1, 'g': 1})
```   
Sin embargo, su ejecución es **secuencial**, lo que puede ser una limitante cuando se procesan grandes volúmenes de datos.  

Este proyecto explora cómo **paralelizar `Counter`** utilizando **MPI en C**. La idea es distribuir la tarea de conteo entre múltiples procesos, lo que permite mejorar la eficiencia en sistemas con múltiples núcleos o clústeres de computación.  

---

## **Justification**  
El problema del conteo de elementos es una tarea común en el procesamiento de datos, pero cuando el volumen de información es grande, una solución secuencial puede ser ineficiente.  

• **Problema:** `Counter` en Python **no escala bien** con grandes volúmenes de datos.  
• **Solución:** Usar **MPI en C** para dividir el trabajo en múltiples procesos y combinar los resultados.  

Esto es especialmente útil en aplicaciones de **procesamiento de texto masivo**, **análisis de logs** o **cómputo científico**, donde se requiere una ejecución rápida y eficiente.  

---

## **General Objective**  
Implementar una versión paralelizada del **contador de frecuencias de `Counter`** en **C con MPI**, distribuyendo el conteo entre múltiples procesos para mejorar la eficiencia en el procesamiento de grandes volúmenes de datos.  

---

## **Particular Objectives**  
✅ Dividir la cadena de texto en partes y distribuirlas entre múltiples procesos MPI.  
✅ Implementar el conteo de caracteres de manera independiente en cada proceso.  
✅ Fusionar los resultados de todos los procesos mediante `MPI_Reduce()`.  
✅ Comparar la eficiencia del método paralelo contra `Counter` en Python.  

---

## **Methodology**  

### **1️⃣ Inicialización**  
• Definir la cadena de texto y el número de procesos MPI.  
• Dividir la cadena en fragmentos iguales y distribuirlos entre procesos.  

### **2️⃣ Implementación del Algoritmo**  
• Cada proceso cuenta los caracteres de su fragmento de forma independiente.  
• Se usa un arreglo de tamaño 256 para almacenar la frecuencia de cada carácter ASCII.  

### **3️⃣ Comunicación y Reducción de Datos**  
• Los procesos envían sus resultados al proceso maestro utilizando `MPI_Reduce()`.  
• El proceso maestro fusiona los conteos parciales para obtener el resultado final.  
