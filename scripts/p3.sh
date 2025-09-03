#!/bin/bash
# Script para ejecutar p3_rw con distintas configuraciones
# y guardar resultados en data/p3_rw.csv

mkdir -p data

# Compilar a .exe dentro de src/
g++ -O2 -pthread src/p3_rw.cpp -o src/p3_rw.exe

# Cabecera CSV
echo "Readers,Writers,OpsPerThread,Time,TotalOps,Throughput" > data/p3_rw.csv

# Configuraciones de prueba
for R in 2 4 8; do
  for W in 1 2; do
    for OPS in 10000 100000; do
      echo "Ejecutando con R=$R W=$W OPS=$OPS..."
      ./src/p3_rw.exe $R $W $OPS >> data/p3_rw.csv
    done
  done
done
