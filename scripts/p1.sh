#!/bin/bash
# Script para p1

mkdir -p data

# Compilar
g++ -pthread src/p1_counter.cpp -o src/p1_counter.exe

# Cabecera CSV
echo "Mode,Threads,Iterations,Time,Throughput" > data/p1_counter.csv

# Configuraciones de prueba
for T in 2 4 8; do
  for I in 100000 1000000; do
    ./src/p1_counter.exe $T $I >> data/p1_counter.csv
  done
done