#!/bin/bash
# Script para generar csv de p2

mkdir -p data

# Compilar a .exe en la raíz
g++ -pthread src/p2_ring.cpp -o src/p2_ring.exe

# Cabecera CSV
echo "Producers,Consumers,N,Time,TotalOps,Throughput" > data/p2_ring.csv

# Configuraciones de prueba
for P in 1 2 4 8; do
  for C in 1 2 4 8; do
    for N in 100000 1000000; do
      echo "Ejecutando con P=$P C=$C N=$N..."
      ./src/p2_ring.exe $P $C $N >> data/p2_ring.csv
    done
  done
done