/*
    Autor: Santiago Cordero
    Fecha de realización: 1/9/2025
    Propósito: Contador global compartido por N hilos con y sin el uso de mutex.
*/

#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>

struct Args { 
    long iters; 
    long* global; 
    pthread_mutex_t* mtx;
}; 

void* worker_naive(void* p){ 
    auto* a = static_cast<Args*>(p);
    for(long i=0;i<a->iters;i++){ 
        (*a->global)++; 
    } // RACE intencionada 
    
    return nullptr;
} 

void* worker_mutex(void* p){
    auto* a = static_cast<Args*>(p); 
    for(long i=0;i<a->iters;i++){ 
        pthread_mutex_lock(a->mtx); 
        (*a->global)++;
        pthread_mutex_unlock(a->mtx); } 
    return nullptr;
} 

int main(int argc, char** argv){ 
    int T = (argc > 1) ? std::atoi(argv[1]) : 4; 
    long it = (argc>2) ? std::atol(argv[2]) : 1000000; 
    long global = 0;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    std::vector<pthread_t> th(T); 
    
    Args a{it, &global, &mtx};
    for(int i=0;i<T;i++) {
        pthread_create(&th[i], nullptr, worker_naive, &a);
    }
    for(int i=0;i<T;i++) {
        pthread_join(th[i], nullptr); 
        std::printf("NAIVE total=%ld (esperado=%ld)", global, (long)T*it);
        std::cout<<std::endl; // Repetir con worker_mutex y comparar tiempos.
    }

    // Utilizando mutex:
    global = 0;
    for (int i = 0; i < T; i++){
        pthread_create(&th[i], nullptr, worker_mutex, &a);
    }

    for (int i = 0; i < T; i++){
        pthread_join(th[i], nullptr);
        std::printf("MUTEX total = %ld (esperado = %ld)", global, (long)T*it);
        std::cout<<std::endl;
    }
}
