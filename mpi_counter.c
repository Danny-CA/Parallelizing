#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text_reader.h"

#define MAX_WORD_LEN 100
#define MAX_WORDS 20000

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

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    char text[MAX_WORDS][MAX_WORD_LEN];
    int total_words = read_words_from_file("input.txt", text);

    int local_size = total_words / world_size;
    int remainder = total_words % world_size;
    int start = world_rank * local_size + (world_rank < remainder ? world_rank : remainder);
    int end = start + local_size + (world_rank < remainder ? 1 : 0);

    WordCount local_counts[MAX_WORDS];
    int local_count_size = 0;

    // Contar localmente
    for (int i = start; i < end; ++i) {
        int index = find_word(local_counts, local_count_size, text[i]);
        if (index != -1) {
            local_counts[index].count++;
        } else {
            strcpy(local_counts[local_count_size].word, text[i]);
            local_counts[local_count_size].count = 1;
            local_count_size++;
        }
    }
    // Recolección en el maestro (rank 0)
    if (world_rank == 0) {
        WordCount global_counts[MAX_WORDS];
        int global_count_size = 0;

        // Copia los propios conteos
        for (int i = 0; i < local_count_size; ++i) {
            global_counts[i] = local_counts[i];
        }
        global_count_size = local_count_size;

        // Recibe los resultados de los demás procesos
        for (int src = 1; src < world_size; ++src) {
            int recv_size;
            MPI_Recv(&recv_size, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            WordCount temp[recv_size];
            MPI_Recv(temp, recv_size * sizeof(WordCount), MPI_BYTE, src, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Fusionar
            for (int j = 0; j < recv_size; ++j) {
                int index = find_word(global_counts, global_count_size, temp[j].word);
                if (index != -1) {
                    global_counts[index].count += temp[j].count;
                } else {
                    global_counts[global_count_size++] = temp[j];
                }
            }
        }

        // Ordenar de mayor a menor por frecuencia
        qsort(global_counts, global_count_size, sizeof(WordCount), compare_wordcount);

        // Mostrar resultados y guardarlos en archivo
        FILE *output_file = fopen("output.txt", "w");
        if (!output_file) {
            perror("Error al abrir archivo de salida");
            MPI_Finalize();
            return 1;
        }
        fprintf(output_file, "Conteo final:\n");
        for (int i = 0; i < global_count_size; ++i) {
            fprintf(output_file, "%s: %d\n", global_counts[i].word, global_counts[i].count);
        }
        fclose(output_file);

    } else {
        // Enviar tamaño primero
        MPI_Send(&local_count_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // Enviar datos
        MPI_Send(local_counts, local_count_size * sizeof(WordCount), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}
