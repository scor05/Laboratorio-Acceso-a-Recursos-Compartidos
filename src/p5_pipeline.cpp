#include <pthread.h>
#include <cstdio>
#include <cstdlib>

constexpr int TICKS = 1000;
static pthread_barrier_t barrier1, barrier2, barrier3, barrier4;
static pthread_once_t once_flag = PTHREAD_ONCE_INIT;

struct Args{
    int* dato;
    bool* isEven;
    long id;
    int* acumulado;
};

FILE* logfile = nullptr;

static void init_shared(){
    /* abrir archivo de log, reservar buffers, etc.*/
    logfile = fopen("data/pipeline_log.txt", "w");
    if (!logfile) {
        perror("Error abriendo log");
        exit(1);
    }
    fprintf(logfile, "Pipeline inicializado\n");
} 

void* stage(void* p){
    Args* arg = static_cast<Args*>(p);
    long id = arg->id;
    bool* isEven = arg->isEven;
    int* num = arg->dato;
    int* acumulado = arg->acumulado;

    pthread_once(&once_flag, init_shared); 
    for(int t=0;t<TICKS;t++){
        /* trabajo */
        /* 
            stage 1 -> Generar aleatorio
            stage 2 -> filtrar impares (que solo sigan al 3 los pares)
            stage 3 -> sumar todos los números filtrados (reducir)
            Poner una barrier antes y después de cada stage para que no se
            adelante ningún hilo.
        */
        pthread_barrier_wait(&barrier1);
        if (id == 1){
            *num = rand() % 100 + 1;
            fprintf(logfile, "Tick #%d generó %d\n", t, *num);
        }
        pthread_barrier_wait(&barrier2);
        if (id == 2){
            if (*num % 2 == 1){
                *isEven = false;
                fprintf(logfile, "Tick #%d filtró %d\n", t, *num);
            } else {
                *isEven = true;
                fprintf(logfile, "Tick #%d dejó pasar a %d\n", t, *num);
            }
        }
        pthread_barrier_wait(&barrier3);
        if (id == 3){
            if (*isEven){
                *acumulado += *num;
                fprintf(logfile, "Tick #%d sumó %d al acumulado: %d\n", t, *num, *acumulado);
            }
        }
        pthread_barrier_wait(&barrier4);
    }

    return nullptr;
}

int main(){
    srand(time(NULL));
    pthread_t h1,h2,h3;
    int global = 0;
    int num;
    bool isEven = false;
    Args* a1 = new Args{&num, &isEven, 1, &global};
    Args* a2 = new Args{&num, &isEven, 2, &global};
    Args* a3 = new Args{&num, &isEven, 3, &global};
    pthread_barrier_init(&barrier1, NULL, 3);
    pthread_barrier_init(&barrier2, NULL, 3);
    pthread_barrier_init(&barrier3, NULL, 3);
    pthread_barrier_init(&barrier4, NULL, 3);
    pthread_create(&h1,0,stage,(void*)a1);
    pthread_create(&h2,0,stage,(void*)a2);
    pthread_create(&h3,0,stage,(void*)a3); 
    pthread_join(h1,0);
    pthread_join(h2,0); 
    pthread_join(h3,0);
    pthread_barrier_destroy(&barrier1);
    pthread_barrier_destroy(&barrier2);
    pthread_barrier_destroy(&barrier3);
    pthread_barrier_destroy(&barrier4);
} 