#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    return wc2->count - wc1->count;  // Orden descendente
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    char filename[128];
    sprintf(filename, "inputs/%d.txt", world_rank + 1);

    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Proceso %d: Error al abrir el archivo %s\n", world_rank, filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Reserva dinámica
    int capacity = 1000;
    WordCount *local_counts = malloc(capacity * sizeof(WordCount));
    if (!local_counts) {
        perror("No se pudo asignar memoria para local_counts");
        fclose(input_file);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    int local_count_size = 0;

    char word[MAX_WORD_LEN];
    while (fscanf(input_file, "%49s", word) == 1) {
        int index = find_word(local_counts, local_count_size, word);
        if (index != -1) {
            local_counts[index].count++;
        } else {
            if (local_count_size >= capacity) {
                capacity *= 2;
                WordCount *tmp = realloc(local_counts, capacity * sizeof(WordCount));
                if (!tmp) {
                    perror("Error al expandir memoria");
                    free(local_counts);
                    fclose(input_file);
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
                local_counts = tmp;
            }
            strncpy(local_counts[local_count_size].word, word, MAX_WORD_LEN);
            local_counts[local_count_size].word[MAX_WORD_LEN - 1] = '\0'; // Seguridad
            local_counts[local_count_size].count = 1;
            local_count_size++;
        }
    }
    fclose(input_file);

    qsort(local_counts, local_count_size, sizeof(WordCount), compare_wordcount);

    // Asegura que la carpeta outputs existe
    system("mkdir -p outputs");

    char output_filename[128];
    sprintf(output_filename, "outputs/output_%d.txt", world_rank + 1);
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Error al abrir archivo de salida");
        free(local_counts);
        MPI_Finalize();
        return 1;
    }

    fprintf(output_file, "Conteo de palabras para archivo %d.txt:\n", world_rank + 1);
    for (int i = 0; i < local_count_size; ++i) {
        fprintf(output_file, "%s: %d\n", local_counts[i].word, local_counts[i].count);
    }
    fclose(output_file);

    free(local_counts);

    double end_time = MPI_Wtime();
    printf("Proceso %d completo en %f segundos\n", world_rank, end_time - start_time);

    MPI_Finalize();
    return 0;
}
