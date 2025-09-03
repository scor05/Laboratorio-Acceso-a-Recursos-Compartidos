/*
    Autor: Santiago Cordero
    Fecha de realización: 2/9/2025
    Propósito: Identificación y correción de un deadlock.
*/

#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <chrono>

pthread_mutex_t A=PTHREAD_MUTEX_INITIALIZER, B=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond;

// Solución con orden global para que todos los hijos ejecuten el código
// en el mismo orden para no causar el deadlock de A y B
void* t1(void*){ 
    pthread_mutex_lock(&A);
    usleep(1000);
    pthread_mutex_lock(&B);
    std::puts("t1 ok");
    pthread_mutex_unlock(&B);
    pthread_mutex_unlock(&A);
    return nullptr;
}

void* t2(void*){ 
    pthread_mutex_lock(&A);
    usleep(1000); 
    pthread_mutex_lock(&B);
    std::puts("t2 ok");
    pthread_mutex_unlock(&B);
    pthread_mutex_unlock(&A);
    return nullptr; 
}

int main(){ 
    pthread_t x,y;

    auto start = std::chrono::high_resolution_clock::now(); 
    pthread_create(&x,nullptr,t1,nullptr);
    pthread_create(&y,nullptr,t2,nullptr); 
    pthread_join(x,nullptr);
    pthread_join(y,nullptr);
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    std::printf("Tiempo total de ejecución (corregido): %.6f s; Throughput: %.6f op/s\n", elapsed, 1/elapsed);

    return 0;
}