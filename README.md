### Martin Merida - 23002494
# Lab 5

## funciones:
- init_stats(LogStats *stats): Inicializa las estructuras de estadísticas y crea los locks utilizados por los threads.

- destroy_stats(LogStats *stats): Libera la memoria usada por las hash tables y destruye los locks.

- lock_mutex(mutex_t *m): Bloquea un mutex para evitar acceso simultáneo a recursos compartidos.

- unlock_mutex(mutex_t *m): Libera un mutex previamente bloqueado.

- parse_log_line(const char *line, char *ip_out, char *url_out, int *status_out): Extrae de una línea del log la IP, URL y status code.

- increment_ip(LogStats *stats, const char *ip): Incrementa el contador de una IP en la tabla hash.

- increment_url(LogStats *stats, const char *url): Incrementa el contador de una URL en la tabla hash.

- process_log_line(LogStats *stats, const char *line): Procesa una línea del log y actualiza las estadísticas correspondientes.

- consume_one_log_entry(const char *filename, LogStats *stats): Lee una línea del archivo log, la procesa y elimina esa línea del archivo.

- print_stats(const LogStats *stats): Imprime las estadísticas finales obtenidas del archivo log.

- worker_thread(): Función ejecutada por cada thread. Cada thread intenta procesar líneas del archivo log hasta que ya no queden más.


Como correr el codigo:

Si se quiere correr la vercion de windows, se tiene que usar MSVC. Dentro de la terminal de MSVC ejecutar lo siguiente:
```bash
run.bat
```
Esto no solo va a correr el codigo del lab, tambien genera 300 logs para que analice el programa.

Para la de linux:
```bash
./run.sh
```
Esto tambien corre el codigo y genera 300 logs
