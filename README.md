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

This project explores how to parallelize `Counter` using MPI in C. Two versions were developed: 
1. One that splits a single text across processes.
2. Another that assigns a separate input file to each process.

---

## **Justification**  
Element counting is a common task in data processing, but when dealing with large datasets, a sequential solution can be inefficient.

• **Problem:** Python’s Counter does not scale well with large datasets.  
• **Solution:** Use MPI in C to split the work across multiple processes and merge the results.

---

## **General Objective**  
Implement two parallelized versions of Python’s Counter in C with MPI: one that distributes a single dataset across processes, and another that assigns different input files to each process. The goal is to improve efficiency in processing large volumes of data using distributed parallelism.

---

## **Particular Objectives**  

• Split a single input file and distribute its content among multiple MPI processes (**splitfile version**).

• Assign separate input files to each MPI process, allowing for distributed data processing from multiple sources (**multifile version**).

• Merge local counting results in the root process to produce a global result.

• Write individual output files per process (in the multifile version) to simplify post-processing.

• Compare the performance of the parallel implementations against Python’s `Counter` with large inputs.
