#ifndef LOG_PROCESSOR_H
#define LOG_PROCESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
typedef CRITICAL_SECTION mutex_t;
#else
#include <pthread.h>
typedef pthread_mutex_t mutex_t;
#endif

#include "uthash.h"

#define MAX_LINE_LEN 512
#define MAX_IP_LEN   64
#define MAX_URL_LEN  256

typedef struct IPEntry {
    char ip[MAX_IP_LEN];
    int count;
    UT_hash_handle hh;
} IPEntry;

typedef struct URLEntry {
    char url[MAX_URL_LEN];
    int count;
    UT_hash_handle hh;
} URLEntry;

typedef struct LogStats {
    IPEntry *ip_table;
    URLEntry *url_table;
    int http_errors;

    mutex_t stats_lock;   // protege hash tables y contador
    mutex_t file_lock;    // protege lectura/modificacion de access.log
} LogStats;

void init_stats(LogStats *stats);
void destroy_stats(LogStats *stats);

void parse_log_line(const char *line, char *ip_out, char *url_out, int *status_out);
void process_log_line(LogStats *stats, const char *line);
int consume_one_log_entry(const char *filename, LogStats *stats);
void print_stats(const LogStats *stats);

#endif