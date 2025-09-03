import pandas as pd
import matplotlib.pyplot as plt

# Leer CSV
df = pd.read_csv("data/p3_rw.csv")

# --- Gráfico 1: Tiempo vs Lectores ---
for ops in df["OpsPerThread"].unique():
    subset_ops = df[df["OpsPerThread"] == ops]
    for w in subset_ops["Writers"].unique():
        subset = subset_ops[subset_ops["Writers"] == w]
        plt.plot(subset["Readers"], subset["Time"], marker="o", label=f"W={w}, Ops={ops}")

plt.xlabel("Lectores")
plt.ylabel("Tiempo (s)")
plt.title("p3_rw - Tiempo vs Lectores")
plt.legend()
plt.grid(True)
plt.savefig("data/p3_rw_time.png")
plt.clf()

# --- Gráfico 2: Throughput vs Lectores ---
for ops in df["OpsPerThread"].unique():
    subset_ops = df[df["OpsPerThread"] == ops]
    for w in subset_ops["Writers"].unique():
        subset = subset_ops[subset_ops["Writers"] == w]
        plt.plot(subset["Readers"], subset["Throughput"], marker="o", label=f"W={w}, Ops={ops}")

plt.xlabel("Lectores")
plt.ylabel("Throughput (ops/s)")
plt.title("p3_rw - Throughput vs Lectores")
plt.legend()
plt.grid(True)
plt.savefig("data/p3_rw_throughput.png")
plt.clf()
