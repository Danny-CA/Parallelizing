# **Parallelizing in C the library `Counter` from python**

This is the final project for the High Performance Computing course at the [<ins>Escuela Nacional de Estudios Superiores UNAM campus Morelia </ins>](https://www.enesmorelia.unam.mx).

---

## License
This project is licensed under the MIT License. Please refer to the LICENSE document for more details regarding referencing and usage of this project.

---

## **Introduction**  
In **Python**, the `Counter` class from the `collections` module allows for efficient element counting:
```python
from collections import Counter  
texto = ["banana", "apple", "orange"] 
conteo = Counter(texto)  
print(conteo)  
# Salida: Counter({'banana': 1, 'apple': 1, 'orange': 1})
```   
However, its execution is sequential, which can be a limitation when processing large amounts of data.

This project explores how to parallelize `Counter` using MPI in C. The idea is to distribute the counting task among multiple processes, improving efficiency on multi-core systems or computing clusters.

---

## **Justification**  
Element counting is a common task in data processing, but when dealing with large datasets, a sequential solution can be inefficient.

• **Problem:** Python’s Counter does not scale well with large datasets.  
• **Solution:** Use MPI in C to split the work across multiple processes and merge the results.

---

## **General Objective**  
Implement a parallelized version of Python’s Counter in C with MPI, distributing the counting task among multiple processes to improve efficiency in processing large amounts of data.

---

## **Particular Objectives**  

✅ Split the text string into parts and distribute them among multiple MPI processes.

✅ Merge the results from all processes using MPI_Reduce().

✅ Compare the efficiency of the parallel method against Python’s Counter.
  
