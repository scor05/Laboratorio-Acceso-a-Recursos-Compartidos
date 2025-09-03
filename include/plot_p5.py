import pandas as pd
import matplotlib.pyplot as plt

# Leer CSV
df = pd.read_csv("data/p5_pipeline.csv")

# --- Gráfico 1: Tiempo vs Ticks ---
plt.plot(df["Ticks"], df["Time"], marker="o", label="Tiempo")
plt.xlabel("Ticks")
plt.ylabel("Tiempo (s)")
plt.title("p5_pipeline - Tiempo vs Ticks")
plt.grid(True)
plt.savefig("data/p5_pipeline_time.png")
plt.clf()

# --- Gráfico 2: Throughput (ops/s) vs Ticks ---
plt.plot(df["Ticks"], df["Ops"], marker="o", label="Ops/s")
plt.xlabel("Ticks")
plt.ylabel("Throughput (ops/s)")
plt.title("p5_pipeline - Throughput vs Ticks (3 ops por tick)")
plt.grid(True)
plt.savefig("data/p5_pipeline_throughput_ops.png")
plt.clf()
