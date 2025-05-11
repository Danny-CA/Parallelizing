// mpi_counter_multifile.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "text_reader.h"

#define MAX_WORD_LEN 50

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordCount;

int find_word(WordCount *array, int size, const char *word) {
    for (int i = 0; i < size; ++i)
        if (strcmp(array[i].word, word) == 0)
            return i;
    return -1;
}

int compare_wordcount(const void *a, const void *b) {
    WordCount *wc1 = (WordCount *)a;
    WordCount *wc2 = (WordCount *)b;
    return wc2->count - wc1->count;  // Descendente
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    char filename[128];
    sprintf(filename, "inputs/%d.txt", world_rank + 1);

    int total_words = 0;
    char (*text)[MAX_WORD_LEN] = read_words_from_file(filename, &total_words);
    if (!text) {
        fprintf(stderr, "Proceso %d: Error al leer el archivo %s\n", world_rank, filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    WordCount *local_counts = malloc(total_words * sizeof(WordCount));
    if (!local_counts) {
        perror("No se pudo asignar memoria para local_counts");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    int local_count_size = 0;

    for (int i = 0; i < total_words; ++i) {
        int index = find_word(local_counts, local_count_size, text[i]);
        if (index != -1) {
            local_counts[index].count++;
        } else {
            strcpy(local_counts[local_count_size].word, text[i]);
            local_counts[local_count_size].count = 1;
            local_count_size++;
        }
    }

    qsort(local_counts, local_count_size, sizeof(WordCount), compare_wordcount);

    char output_filename[128];
    sprintf(output_filename, "outputs/output_%d.txt", world_rank + 1);
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Error al abrir archivo de salida");
        MPI_Finalize();
        return 1;
    }

    fprintf(output_file, "Conteo de palabras para archivo %d.txt:\n", world_rank + 1);
    for (int i = 0; i < local_count_size; ++i) {
        fprintf(output_file, "%s: %d\n", local_counts[i].word, local_counts[i].count);
    }
    fclose(output_file);

    free(local_counts);
    free(text);

    double end_time = MPI_Wtime();
    printf("Proceso %d completo en %f segundos\n", world_rank, end_time - start_time);

    MPI_Finalize();
    return 0;
}
