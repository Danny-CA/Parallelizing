import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("data/c_small.csv")

# Graficar
plt.figure(figsize=(12, 6))
plt.plot(df["Iteración"], df["Tiempo"], marker='o', linestyle='-', color='navy')
plt.title("Tiempos de ejecución en 100 repeticiones", fontsize=14)
plt.xlabel("Iteración")
plt.ylabel("Tiempo (s)")
plt.grid(True)
plt.show()
