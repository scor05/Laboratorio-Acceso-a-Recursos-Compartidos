#include <pthread.h>
#include <vector>
#include <cstdint> 

constexpr int NBUCKET = 1024;

struct Node{ 
    int k,v;
    Node* next; 
};

struct Map { 
    Node* b[NBUCKET]{}; 
    pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER; 
}; 

int map_get(Map* m, int k){
    pthread_rwlock_rdlock(&m->rw); 
    /* ... */
    pthread_rwlock_unlock(&m->rw); 
    return 0; 
}

void map_put(Map* m, int k, int v){ 
    pthread_rwlock_wrlock(&m->rw); 
    /* ... */
    pthread_rwlock_unlock(&m->rw); 
}