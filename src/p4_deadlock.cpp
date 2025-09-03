#include <pthread.h>
#include <unistd.h>
#include <cstdio> 

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
    pthread_create(&x,nullptr,t1,nullptr);
    pthread_create(&y,nullptr,t2,nullptr); 
    pthread_join(x,nullptr);
    pthread_join(y,nullptr); 
}