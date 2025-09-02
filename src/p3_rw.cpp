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
#include <ctime>

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
    int num;
};

int customHash(int k){
    return k % NBUCKET; // Para que siempre esté definido k en la bucket "n"
}

// Para medir el throughput = operaciones / tiempo
double now(){
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
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
    
    // Leer un dato con clave al azar del map:
    int k = rand() % NBUCKET;
    int v = map_get(m, k);
    // printf("Leído el valor %d de la clave %d en el map\n", v, k);
    a->num = v;

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
    // distribuciones de carga 90/10, 70/30, 50/50:
    vector<int> readLoads = {900,700,500};
    vector<int> writeLoads = {100,300,500};

    for (int i = 0; i < readLoads.size(); i++){
        double start = now();
        printf("\nDistribución de carga: %d/%d\n\n", readLoads[i]/10, writeLoads[i]/10);
        
        pthread_t readID[readLoads[i]];
        pthread_t writeID[writeLoads[i]];
        Map* m = new Map;
        int num;
        for (int j = 0; j < readLoads[i]; j++){
            Args* a = new Args{m, num};
            pthread_create(&readID[j], nullptr, readerThread, a);
        }
        for (int j = 0; j < writeLoads[i]; j++){
            Args* a = new Args{m, num};
            pthread_create(&writeID[j], nullptr, writerThread, a);
        }

        // que primero se escriba y luego lea:
        for (int j = 0; j < writeLoads[i]; j++){
            pthread_join(writeID[j], nullptr);
        }
        for (int j = 0; j < readLoads[i]; j++){
            pthread_join(readID[j], nullptr);
        }
        double end = now();
        double elapsed = end - start;
        // operaciones siempre son 100 en total porque se crean 100 hilos para
        // todas las iteraciones, por lo que throughput = ops/tiempo = 100/tiempo
        printf("Duración: %.6f s, Throughput: %.6f operaciones/s", elapsed, 100 / elapsed);
        printf("\n--------------------------------");

        delete m;
        
    }
    printf("\n");

    
}