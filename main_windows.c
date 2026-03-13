#include <stdio.h>
#include <windows.h>
#include "log_processor.h"

typedef struct {
    int thread_id;
    LogStats *stats;
    const char *filename;
} ThreadArgs;

DWORD WINAPI worker_thread(LPVOID arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    while (consume_one_log_entry(args->filename, args->stats)) {
    }
    
    return 0;
}

int main(void) {
    HANDLE threads[3];
    ThreadArgs args[3];
    LogStats stats;

    init_stats(&stats);

    for (int i = 0; i < 3; i++) {
        args[i].thread_id = i + 1;
        args[i].stats = &stats;
        args[i].filename = "access.log";

        threads[i] = CreateThread(NULL, 0, worker_thread, &args[i], 0, NULL);

        if (threads[i] == NULL) {
            fprintf(stderr, "Error creando thread %d\n", i + 1);

            for (int j = 0; j < i; j++) {
                WaitForSingleObject(threads[j], INFINITE);
                CloseHandle(threads[j]);
            }

            destroy_stats(&stats);
            return 1;
        }
    }

    WaitForMultipleObjects(3, threads, TRUE, INFINITE);

    for (int i = 0; i < 3; i++) {
        CloseHandle(threads[i]);
    }

    print_stats(&stats);
    destroy_stats(&stats);

    return 0;
}