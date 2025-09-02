#include <pthread.h>
#include <cstdio>
#include <cstddef>
#include <iostream>

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
    
void ring_push(Ring* r, int v){ 
    pthread_mutex_lock(&r->m); 
    
    while(r->count == Q && !r->stop){
        pthread_cond_wait(&r->not_full,&r->m);
        if(!r->stop){ 
            r->buf[r->head]=v; 
            r->head=(r->head+1) % Q; 
            r->count++; 
            pthread_cond_signal(&r->not_empty); 
        }
        pthread_mutex_unlock(&r->m);
    }
}

bool ring_pop(Ring* r, int* out){
    pthread_mutex_lock(&r->m); 
    while(r->count==0 && !r->stop){
        pthread_cond_wait(&r->not_empty,&r->m); 
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
}

int main(int argc, char** argv){
    Ring* r = new Ring;
    // primer parámetro de la ejecución dicta cuántos hilos tratarán de sacar de la cola
    int producer = (argc > 1) ? atoi(argv[1]) : 1;

    pthread_t ids[producer];
    
    for (int i = 0; i < producer; i++){
        pthread_create(&ids[i], nullptr, ring_push, r);
    }

}