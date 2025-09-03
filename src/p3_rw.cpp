/*
    Autor: Santiago Cordero
    Fecha de realización: 1/9/2025
    Propósito: Implementación de un HashMap con hilos para lectura y escritura.
*/

#include <pthread.h>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <chrono>

constexpr int NBUCKET = 1024;
using namespace std;

struct Node{ 
    int k,v;
    Node* next; 
};

struct Map { 
    Node* b[NBUCKET]{}; 
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER; 
};

struct Args{
    Map* m;
    int ops;
};

int customHash(int k){
    return k % NBUCKET; // Para que siempre esté definido k en la bucket "n"
}


int map_get(Map* m, int k){
    pthread_rwlock_rdlock(&m->rw); 
    int index = customHash(k); // representa el índice de la bucket en la que está el dato
    Node* cur = m->b[index];
    // Iterar hasta que llegue al final (donde cur->next == nullptr)
    while (cur){
        if (cur->k == k){
            int get = cur->v;
            pthread_rwlock_unlock(&m->rw);
            return get;
        }
        cur = cur->next;
    }
    pthread_rwlock_unlock(&m->rw); 
    return 0; 
}

void map_put(Map* m, int k, int v){ 
    pthread_rwlock_wrlock(&m->rw); 
    int index = customHash(k);
    Node* curr = m->b[index];
    while (curr){
        if (curr->k == k){
            curr->v = v;
            pthread_rwlock_unlock(&m->rw);
            return;
        }
        curr = curr->next;
    }
    Node* newNode = new Node{k, v, m->b[index]}; // caso de que no se encuentre en ninguna bucket.
    m->b[index] = newNode;
    pthread_rwlock_unlock(&m->rw); 
}

void* readerThread(void* arg){
    Args* a = static_cast<Args*>(arg);
    Map* m = a->m;

    for (int i = 0; i < a->ops; i++) {
        int k = rand() % 10000;
        int v = rand() % 10000;
        map_put(a->m, k, v);
    }

    return nullptr;
}

void* writerThread(void* arg){
    Args* a = static_cast<Args*>(arg);
    Map* m = a->m;

    // Reescribir el value de un nodo al azar del map
    int k = rand() % NBUCKET;
    int replace = rand() % 100 + 1; // reemplazar por un valor aleatorio
    // printf("Reemplazando %d en clave %d en el map\n", replace, k);
    map_put(m, k, replace);

    return nullptr;
}

int main(int argc, char** argv){
    srand(time(NULL));

    // por default escoge 90/10 de distribución
    int R = (argc > 1) ? std::atoi(argv[1]) : 9;   // número de lectores
    int W = (argc > 2) ? std::atoi(argv[2]) : 1;   // número de escritores
    int OPS = (argc > 3) ? std::atoi(argv[3]) : 100000; // operaciones por hilo

    Map m;
    std::vector<pthread_t> threads;
    threads.reserve(R + W);

    Args a{&m, OPS};

    auto start = std::chrono::high_resolution_clock::now();

    // lanzar lectores
    for (int i = 0; i < R; i++) {
        pthread_t t;
        pthread_create(&t, nullptr, readerThread, &a);
        threads.push_back(t);
    }

    // lanzar escritores
    for (int i = 0; i < W; i++) {
        pthread_t t;
        pthread_create(&t, nullptr, writerThread, &a);
        threads.push_back(t);
    }

    // esperar hilos
    for (auto& t : threads) {
        pthread_join(t, nullptr);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();

    long total_ops = (R + W) * OPS;
    double throughput = total_ops / elapsed;

    // std::printf("Lectores=%d, Escritores=%d, Operaciones por hilo=%d\n", R, W, OPS);
    // std::printf("Tiempo total: %.6f s\n", elapsed);
    // std::printf("Operaciones realizadas: %ld\n", total_ops);
    // std::printf("Throughput: %.2f ops/s\n", throughput);

    std::printf("%d,%d,%d,%.6f,%ld,%.2f\n", R, W, OPS, elapsed, total_ops, throughput);

    return 0;
}