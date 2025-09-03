import pandas as pd
import matplotlib.pyplot as plt
import os

# Leer CSV
df = pd.read_csv("data/p2_ring.csv")

# Gráfica Tiempo vs Consumidores
for n in df["N"].unique():
    subset_n = df[df["N"] == n]
    for p in subset_n["Producers"].unique():
        subset = subset_n[subset_n["Producers"] == p]
        plt.plot(subset["Consumers"], subset["Time"], marker="o", label=f"P={p}, N={n}")

plt.xlabel("Consumidores")
plt.ylabel("Tiempo (s)")
plt.title("p2_ring - Tiempo vs Consumidores")
plt.legend()
plt.grid(True)
plt.savefig("data/p2_ring_time.png")
plt.clf()

# Gráfico 2: Throughput vs Consumidores
for n in df["N"].unique():
    subset_n = df[df["N"] == n]
    for p in subset_n["Producers"].unique():
        subset = subset_n[subset_n["Producers"] == p]
        plt.plot(subset["Consumers"], subset["Throughput"], marker="o", label=f"P={p}, N={n}")

plt.xlabel("Consumidores")
plt.ylabel("Throughput (ops/s)")
plt.title("p2_ring - Throughput vs Consumidores")
plt.legend()
plt.grid(True)
plt.savefig("data/p2_ring_throughput.png")
plt.clf()
