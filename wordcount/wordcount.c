#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <omp.h>
#include "uthash.h"
#include "wordcount.h"

typedef struct {
    char word[64];
    size_t count;
    UT_hash_handle hh;
} WordEntry;

const char *stopwords[] = {
    "the", "and", "to", "of", "in", "a", "is", "that", "it", "on", "was", "he",
    "with", "as", "for", "his", "they", "be", "at", "one", "have", "this", "from", NULL
};

int is_stopword(const char *word) {
    for (int i = 0; stopwords[i] != NULL; i++) {
        if (strcmp(word, stopwords[i]) == 0) return 1;
    }
    return 0;
}

void normalize_word(char *word) {
    int j = 0;
    for (int i = 0; word[i]; i++) {
        if (isalnum(word[i])) {
            word[j++] = tolower(word[i]);
        }
    }
    word[j] = '\0';
}

void count_words_map(const char *data, size_t start, size_t end, WordEntry **local_map) {
    char word[64];
    int idx = 0;
    for (size_t i = start; i < end; i++) {
        if (isalnum(data[i])) {
            if (idx < 63) word[idx++] = data[i];
        } else {
            if (idx > 0) {
                word[idx] = '\0';
                normalize_word(word);
                if (!is_stopword(word) && strlen(word) > 0) {
                    WordEntry *entry;
                    HASH_FIND_STR(*local_map, word, entry);
                    if (entry) {
                        entry->count++;
                    } else {
                        entry = malloc(sizeof(WordEntry));
                        strcpy(entry->word, word);
                        entry->count = 1;
                        HASH_ADD_STR(*local_map, word, entry);
                    }
                }
                idx = 0;
            }
        }
    }
}

void parallel_word_count(const char *filename, int num_threads) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        exit(1);
    }

    size_t filesize = st.st_size;
    char *data = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    WordEntry *global_map = NULL;

    omp_set_num_threads(num_threads);
    #pragma omp parallel
    {
        WordEntry *local_map = NULL;
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();

        size_t chunk_size = filesize / nthreads;
        size_t start = tid * chunk_size;
        size_t end = (tid == nthreads - 1) ? filesize : start + chunk_size;

        if (tid != 0) {
            while (start < end && !isspace(data[start])) start++;
        }

        count_words_map(data, start, end, &local_map);

        // Fusionar mapa local al global
        #pragma omp critical
        {
            WordEntry *entry, *tmp;
            HASH_ITER(hh, local_map, entry, tmp) {
                WordEntry *existing;
                HASH_FIND_STR(global_map, entry->word, existing);
                if (existing) {
                    existing->count += entry->count;
                } else {
                    existing = malloc(sizeof(WordEntry));
                    strcpy(existing->word, entry->word);
                    existing->count = entry->count;
                    HASH_ADD_STR(global_map, word, existing);
                }
                HASH_DEL(local_map, entry);
                free(entry);
            }
        }
    }

    // Guardar en archivo
    FILE *out = fopen("result.txt", "w");
    if (!out) {
        perror("fopen");
        munmap(data, filesize);
        close(fd);
        exit(1);
    }

    WordEntry *entry, *tmp;
    HASH_ITER(hh, global_map, entry, tmp) {
        fprintf(out, "%s: %zu\n", entry->word, entry->count);
        HASH_DEL(global_map, entry);
        free(entry);
    }

    fclose(out);
    munmap(data, filesize);
    close(fd);
}
