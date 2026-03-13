#include <pthread.h>
#include <stdio.h>
#include "log_processor.h"

#define NUM_THREADS 3

typedef struct {
    int thread_id;
    LogStats *stats;
    const char *filename;
} ThreadArgs;

void *worker_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    while (consume_one_log_entry(args->filename, args->stats)) {
    }
}

int main(void) {
    pthread_t threads1;
    ThreadArgs args1;
    pthread_t threads2;
    ThreadArgs args2;
    pthread_t threads3;
    ThreadArgs args3;

    LogStats stats;

    init_stats(&stats);

    args1.thread_id = 1;
    args1.stats = &stats;
    args1.filename = "access.log";

    if (pthread_create(&threads1, NULL, worker_thread, &args1) != 0) {
        perror("Error creando thread");
        destroy_stats(&stats);
        return 1;
    }

    args2.thread_id = 1;
    args2.stats = &stats;
    args2.filename = "access.log";

    if (pthread_create(&threads2, NULL, worker_thread, &args2) != 0) {
        perror("Error creando thread");
        destroy_stats(&stats);
        return 1;
    }

    args3.thread_id = 1;
    args3.stats = &stats;
    args3.filename = "access.log";

    if (pthread_create(&threads3, NULL, worker_thread, &args3) != 0) {
        perror("Error creando thread");
        destroy_stats(&stats);
        return 1;
    }

    pthread_join(threads1, NULL);
    pthread_join(threads2, NULL);
    pthread_join(threads3, NULL);

    print_stats(&stats);
    destroy_stats(&stats);

    return 0;
}