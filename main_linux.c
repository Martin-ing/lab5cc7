#include <pthread.h>
#include <stdio.h>
#include "log_processor.h"

#define NUM_THREADS 3

// Estructura para pasar argumentos a cada thread
typedef struct {
    int thread_id;
    LogStats *stats;
    const char *filename;
} ThreadArgs;

// Función que ejecutan los threads para procesar líneas del log
void *worker_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    // Cada thread intenta consumir líneas del log hasta que ya no queden
    while (consume_one_log_entry(args->filename, args->stats)) {
    }
}

// Función principal donde se crean y controlan los threads
int main(void) {
    pthread_t threads1;
    ThreadArgs args1;
    pthread_t threads2;
    ThreadArgs args2;
    pthread_t threads3;
    ThreadArgs args3;

    LogStats stats;

    // Inicializa las estructuras de estadísticas
    init_stats(&stats);

    // Configura los argumentos del primer thread
    args1.thread_id = 1;
    args1.stats = &stats;
    args1.filename = "access.log";

    // Crea el primer thread
    if (pthread_create(&threads1, NULL, worker_thread, &args1) != 0) {
        perror("Error creando thread");
        destroy_stats(&stats);
        return 1;
    }

    // Configura los argumentos del segundo thread
    args2.thread_id = 1;
    args2.stats = &stats;
    args2.filename = "access.log";

    // Crea el segundo thread
    if (pthread_create(&threads2, NULL, worker_thread, &args2) != 0) {
        perror("Error creando thread");
        destroy_stats(&stats);
        return 1;
    }

    // Configura los argumentos del tercer thread
    args3.thread_id = 1;
    args3.stats = &stats;
    args3.filename = "access.log";

    // Crea el tercer thread
    if (pthread_create(&threads3, NULL, worker_thread, &args3) != 0) {
        perror("Error creando thread");
        destroy_stats(&stats);
        return 1;
    }

    // Espera a que todos los threads terminen su ejecución
    pthread_join(threads1, NULL);
    pthread_join(threads2, NULL);
    pthread_join(threads3, NULL);

    // Muestra las estadísticas finales
    print_stats(&stats);

    // Libera memoria y destruye las estructuras usadas
    destroy_stats(&stats);

    return 0;
}