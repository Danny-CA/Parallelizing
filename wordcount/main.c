#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "wordcount.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <archivo.txt> <num_hilos>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int num_threads = atoi(argv[2]);

    if (num_threads <= 0) {
        fprintf(stderr, "Número de hilos inválido.\n");
        return 1;
    }

    clock_t start = clock();

    parallel_word_count(filename, num_threads);

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Tiempo total: %.3f segundos\n", elapsed);

    return 0;
}
