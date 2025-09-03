/*
    Autor: Santiago Cordero
    Fecha de realización: 1/9/2025
    Propósito: Implementación de Cola SPSC, SPMC con un hilo para el producidor y varios consumidores.
*/

#include <pthread.h>
#include <cstdio>
#include <cstddef>
#include <iostream>
#include <chrono>

constexpr std::size_t Q = 1024;

using namespace std;

struct Ring { 
    int buf[Q]; 
    size_t head=0, tail=0, count=0; 
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; 
    pthread_cond_t not_full = PTHREAD_COND_INITIALIZER; 
    pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER; 
    bool stop=false;
};

struct Args {
    Ring* r;
    long n; // cuántos elementos produce/consume
};
    
void ring_push(Ring* r, int v){ 
    pthread_mutex_lock(&r->m); 
    
    while(r->count == Q && !r->stop){
        pthread_cond_wait(&r->not_full,&r->m);
    }
    if(!r->stop){ 
        r->buf[r->head]=v; 
        r->head=(r->head+1) % Q; 
        r->count++; 
        pthread_cond_signal(&r->not_empty); 
    }
    pthread_mutex_unlock(&r->m);
}


bool ring_pop(Ring* r, int* out){
    pthread_mutex_lock(&r->m); 
    while(r->count==0 && !r->stop){
        pthread_cond_wait(&r->not_empty,&r->m);
    }
    if(r->count==0 && r->stop){ 
        pthread_mutex_unlock(&r->m); 
        return false; 
    } 
    *out=r->buf[r->tail]; 
    r->tail=(r->tail+1)%Q; 
    r->count--;
    pthread_cond_signal(&r->not_full); 
    pthread_mutex_unlock(&r->m);
    return true;
}

void* producerThread(void* arg){
    Args* a = static_cast<Args*>(arg);
    for (int i = 0; i < a->n; i++){
        ring_push(a->r, i);
    }
    return nullptr;
}

void* consumerThread(void* arg){
    Args* a = static_cast<Args*>(arg);
    int val;
    for(long i=0; i< a->n; i++){
        if(!ring_pop(a->r, &val)) 
        break;
    }
    return nullptr;
}

int main(int argc, char** argv){
    int P = (argc > 1) ? std::atoi(argv[1]) : 1; // # productores
    int C = (argc > 2) ? std::atoi(argv[2]) : 1; // # consumidores
    long N = (argc > 3) ? std::atol(argv[3]) : 1000000; // elementos por productor

    Ring* r = new Ring;

    pthread_t prod[P], cons[C];
    Args ap{r, N};
    Args ac{r, (N*P)/C}; // cada consumidor balancea el trabajo

    auto start = std::chrono::high_resolution_clock::now();

    // lanzar productores
    for(int i=0;i<P;i++){
        pthread_create(&prod[i], nullptr, producerThread, &ap);
    }
    // lanzar consumidores
    for(int i=0;i<C;i++){
        pthread_create(&cons[i], nullptr, consumerThread, &ac);
    }

    // esperar productores
    for(int i=0;i<P;i++){
        pthread_join(prod[i], nullptr);
    }

    // marcar stop para que consumidores terminen
    pthread_mutex_lock(&r->m);
    r->stop = true;
    pthread_cond_broadcast(&r->not_empty);
    pthread_mutex_unlock(&r->m);

    // esperar consumidores
    for(int i=0;i<C;i++){
        pthread_join(cons[i], nullptr);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();

    long total_ops = P * N;
    double throughput = total_ops / elapsed;

    std::printf("Productores=%d, Consumidores=%d, N=%ld\n", P, C, N);
    std::printf("Tiempo total: %.6f s\n", elapsed);
    std::printf("Operaciones procesadas: %ld\n", total_ops);
    std::printf("Throughput: %.2f ops/s\n", throughput);

    // prints para el script de bash
    // std::printf("%d,%d,%ld,%.6f,%ld,%.2f\n", P, C, N, elapsed, total_ops, throughput);
    delete r;
    return 0;
}