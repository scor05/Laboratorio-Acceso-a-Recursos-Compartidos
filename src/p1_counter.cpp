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
#include <chrono>

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
    long total_op = T * it;
    
    // Caso naive
    global = 0;
    auto start_naive = std::chrono::high_resolution_clock::now();
    for(int i=0;i<T;i++) {
        pthread_create(&th[i], nullptr, worker_naive, &a);
    }
    for(int i=0;i<T;i++) {
        pthread_join(th[i], nullptr); 
    }
    auto end_naive = std::chrono::high_resolution_clock::now();
    double time_naive = std::chrono::duration<double>(end_naive - start_naive).count();
    double throughput_naive = total_op/time_naive;
    std::printf("NAIVE total=%ld (esperado=%ld) tiempo=%.6fs throughput=%.3f op/s\n", global, total_op, time_naive, throughput_naive);

    // Caso mutex
    global = 0;
    auto start_mutex = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < T; i++){
        pthread_create(&th[i], nullptr, worker_mutex, &a);
    }
    for (int i = 0; i < T; i++){
        pthread_join(th[i], nullptr);
    }
    auto end_mutex = std::chrono::high_resolution_clock::now();
    double time_mutex = std::chrono::duration<double>(end_mutex - start_mutex).count();
    double throughput_mutex = total_op/time_mutex;
    std::printf("MUTEX total=%ld (esperado=%ld) tiempo=%.6fs throughput=%.3f op/s\n", global, total_op, time_mutex, throughput_mutex);

    // Cálculo de slowdown (fracción de mutex/naive)
    if (time_naive > 0) {
        double slowdown = time_mutex / time_naive;
        std::printf("Slowdown (mutex vs naive): %.2f%c \n", slowdown*100, '%');
    }
}
