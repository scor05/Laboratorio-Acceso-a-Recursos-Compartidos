import pandas as pd
import matplotlib.pyplot as plt
import os

# Leer CSV
df = pd.read_csv("data/p1_counter.csv")

# Gráfica tiempo/threads
for it in df["Iterations"].unique():
    subset = df[df["Iterations"] == it]
    for mode in ["naive", "mutex"]:
        data = subset[subset["Mode"] == mode]
        plt.plot(data["Threads"], data["Time"], marker="o", label=f"{mode} it={it}")

plt.xlabel("Threads")
plt.ylabel("Tiempo (s)")
plt.title("p1_counter - Tiempo vs Threads")
plt.legend()
plt.grid(True)
plt.savefig("data/p1_counter_time.png")
plt.clf()

# Gráfica throughput/threads
for it in df["Iterations"].unique():
    subset = df[df["Iterations"] == it]
    for mode in ["naive", "mutex"]:
        data = subset[subset["Mode"] == mode]
        plt.plot(data["Threads"], data["Throughput"], marker="o", label=f"{mode} it={it}")

plt.xlabel("Threads")
plt.ylabel("Throughput (ops/s)")
plt.title("p1_counter - Throughput vs Threads")
plt.legend()
plt.grid(True)
plt.savefig("data/p1_counter_throughput.png")
plt.clf()
