#!/bin/bash
# Script para ejecutar p5_pipeline con distintas configuraciones
# y guardar resultados en data/p5_pipeline.csv

mkdir -p data

# Compilar a .exe en src/
g++ -pthread src/p5_pipeline.cpp -o src/p5_pipeline.exe

# Cabecera CSV
echo "Ticks,Time,Ops,Acumulado" > data/p5_pipeline.csv

# Configuraciones de prueba
for T in 100 1000 5000 10000; do
  echo "Ejecutando con TICKS=$T..."
  ./src/p5_pipeline.exe $T >> data/p5_pipeline.csv
done
