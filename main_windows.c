#include <stdio.h>
#include <windows.h>
#include "log_processor.h"

// Estructura usada para pasar argumentos a cada thread
typedef struct {
    int thread_id;
    LogStats *stats;
    const char *filename;
} ThreadArgs;

// Función que ejecuta cada thread para procesar líneas del log
DWORD WINAPI worker_thread(LPVOID arg) {
    ThreadArgs *args = (ThreadArgs *)arg;

    // Cada thread intenta procesar líneas del archivo hasta que ya no queden
    while (consume_one_log_entry(args->filename, args->stats)) {
    }
    
    return 0;
}

// Función principal donde se crean y manejan los threads
int main(void) {
    HANDLE threads[3];
    ThreadArgs args[3];
    LogStats stats;

    // Inicializa las estructuras de estadísticas
    init_stats(&stats);

    // Crea los threads que procesarán el log
    for (int i = 0; i < 3; i++) {
        args[i].thread_id = i + 1;
        args[i].stats = &stats;
        args[i].filename = "access.log";

        threads[i] = CreateThread(NULL, 0, worker_thread, &args[i], 0, NULL);

        // Si ocurre un error creando un thread
        if (threads[i] == NULL) {
            fprintf(stderr, "Error creando thread %d\n", i + 1);

            // Espera a que terminen los threads que sí se crearon
            for (int j = 0; j < i; j++) {
                WaitForSingleObject(threads[j], INFINITE);
                CloseHandle(threads[j]);
            }

            destroy_stats(&stats);
            return 1;
        }
    }

    // Espera a que todos los threads terminen
    WaitForMultipleObjects(3, threads, TRUE, INFINITE);

    // Cierra los handles de los threads
    for (int i = 0; i < 3; i++) {
        CloseHandle(threads[i]);
    }

    // Imprime las estadísticas finales
    print_stats(&stats);

    // Libera recursos usados por las estadísticas
    destroy_stats(&stats);

    return 0;
}