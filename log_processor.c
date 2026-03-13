#include "log_processor.h"

static void lock_mutex(mutex_t *m) {
#ifdef _WIN32
    EnterCriticalSection(m);
#else
    pthread_mutex_lock(m);
#endif
}

static void unlock_mutex(mutex_t *m) {
#ifdef _WIN32
    LeaveCriticalSection(m);
#else
    pthread_mutex_unlock(m);
#endif
}

void init_stats(LogStats *stats) {
    stats->ip_table = NULL;
    stats->url_table = NULL;
    stats->http_errors = 0;

#ifdef _WIN32
    InitializeCriticalSection(&stats->stats_lock);
    InitializeCriticalSection(&stats->file_lock);
#else
    pthread_mutex_init(&stats->stats_lock, NULL);
    pthread_mutex_init(&stats->file_lock, NULL);
#endif
}


void destroy_stats(LogStats *stats) {
    IPEntry *ip_curr, *ip_tmp;
    URLEntry *url_curr, *url_tmp;

    HASH_ITER(hh, stats->ip_table, ip_curr, ip_tmp) {
        HASH_DEL(stats->ip_table, ip_curr);
        free(ip_curr);
    }

    HASH_ITER(hh, stats->url_table, url_curr, url_tmp) {
        HASH_DEL(stats->url_table, url_curr);
        free(url_curr);
    }
#ifdef _WIN32
    DeleteCriticalSection(&stats->stats_lock);
    DeleteCriticalSection(&stats->file_lock);
#else
    pthread_mutex_destroy(&stats->stats_lock);
    pthread_mutex_destroy(&stats->file_lock);
#endif
}

void parse_log_line(const char *line, char *ip_out, char *url_out, int *status_out) {
    char method[32];
    char timestamp[128];

    sscanf(line,"%63s - - [%127[^]]] \"%31s %255[^\"]\" %d",ip_out,timestamp,method,url_out,status_out);
}

static void increment_ip(LogStats *stats, const char *ip) {
    IPEntry *entry = NULL;
    HASH_FIND_STR(stats->ip_table, ip, entry);

    if (entry == NULL) {
        entry = (IPEntry *)malloc(sizeof(IPEntry));
        strncpy(entry->ip, ip, MAX_IP_LEN - 1);
        entry->ip[MAX_IP_LEN - 1] = '\0';
        entry->count = 1;
        HASH_ADD_STR(stats->ip_table, ip, entry);
    } else {
        entry->count++;
    }
}

static void increment_url(LogStats *stats, const char *url) {
    URLEntry *entry = NULL;
    HASH_FIND_STR(stats->url_table, url, entry);

    if (entry == NULL) {
        entry = (URLEntry *)malloc(sizeof(URLEntry));
        strncpy(entry->url, url, MAX_URL_LEN - 1);
        entry->url[MAX_URL_LEN - 1] = '\0';
        entry->count = 1;
        HASH_ADD_STR(stats->url_table, url, entry);
    } else {
        entry->count++;
    }
}

void process_log_line(LogStats *stats, const char *line) {
    char ip[MAX_IP_LEN];
    char url[MAX_URL_LEN];
    int status;

    parse_log_line(line, ip, url, &status);

    lock_mutex(&stats->stats_lock);

    increment_ip(stats, ip);
    increment_url(stats, url);

    if (status >= 400 && status <= 599) {
        stats->http_errors++;
    }

    unlock_mutex(&stats->stats_lock);
}

int consume_one_log_entry(const char *filename, LogStats *stats) {
    lock_mutex(&stats->file_lock);

    FILE *src = fopen(filename, "r");

    FILE *tmp = fopen("access.tmp", "w");

    char line[MAX_LINE_LEN];
    int processed = 0;

    while (fgets(line, sizeof(line), src)) {
        if (!processed) {
            process_log_line(stats, line);
            processed = 1;
        } else {
            fputs(line, tmp);
        }
    }

    fclose(src);
    fclose(tmp);

    if (!processed) {
        remove("access.tmp");
        unlock_mutex(&stats->file_lock);
        return 0;
    }

    if (remove(filename) != 0) {
        unlock_mutex(&stats->file_lock);
        perror("Error eliminando archivo original");
        return 0;
    }

    if (rename("access.tmp", filename) != 0) {
        unlock_mutex(&stats->file_lock);
        perror("Error renombrando archivo temporal");
        return 0;
    }

    unlock_mutex(&stats->file_lock);
    return 1;
}

void print_stats(const LogStats *stats) {
    printf("IPs unicas: %u\n", HASH_COUNT(stats->ip_table));
    printf("Errores HTTP (400-599): %d\n", stats->http_errors);

    const URLEntry *entry, *top = NULL;
    for (entry = stats->url_table; entry != NULL; entry = entry->hh.next) {
        if (top == NULL || entry->count > top->count) {
            top = entry;
        }
    }

    if (top) {
        printf("URL mas visitada: %s (%d veces)\n", top->url, top->count);
    } else {
        printf("URL mas visitada: N/A\n");
    }
}