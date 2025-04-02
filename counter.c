#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 50
#define MAX_WORDS 10000

// Simple struct to store word counts
typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordCount;

int find_word(WordCount *words, int word_count, const char *word) {
    for (int i = 0; i < word_count; i++) {
        if (strcmp(words[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

void count_words(const char *text, WordCount *words, int *word_count) {
    char buffer[MAX_WORD_LEN];
    int index = 0;
    
    while (*text) {
        if (isalpha(*text)) {
            buffer[index++] = tolower(*text);
            if (index >= MAX_WORD_LEN - 1) index = MAX_WORD_LEN - 2;
        } else if (index > 0) {
            buffer[index] = '\0';
            index = 0;
            int pos = find_word(words, *word_count, buffer);
            if (pos == -1 && *word_count < MAX_WORDS) {
                strcpy(words[*word_count].word, buffer);
                words[*word_count].count = 1;
                (*word_count)++;
            } else {
                words[pos].count++;
            }
        }
        text++;
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *text = "This is an example text for MPI word counting. MPI is powerful.";
    int text_len = strlen(text);
    int chunk_size = text_len / size;
    char *local_text = (char *)malloc((chunk_size + 1) * sizeof(char));
    
    MPI_Scatter(text, chunk_size, MPI_CHAR, local_text, chunk_size, MPI_CHAR, 0, MPI_COMM_WORLD);
    local_text[chunk_size] = '\0';
    
    WordCount local_counts[MAX_WORDS];
    int local_word_count = 0;
    count_words(local_text, local_counts, &local_word_count);

    // Reduce to a global count
    WordCount global_counts[MAX_WORDS];
    int global_word_count = 0;
    MPI_Reduce(&local_counts, &global_counts, MAX_WORDS * sizeof(WordCount), MPI_BYTE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
    printf("Word Counts:\n");
    for (int i = 0; i < MAX_WORDS; i++) { // Imprime todas las palabras
        if (global_counts[i].count > 0) {
            printf("%s: %d\n", global_counts[i].word, global_counts[i].count);
        }
    }
}

// Imprimir en cada proceso para ver qué está contando
printf("Proceso %d:\n", rank);
for (int i = 0; i < local_word_count; i++) {
    printf("%s: %d\n", local_counts[i].word, local_counts[i].count);
}

    
    free(local_text);
    MPI_Finalize();
    return 0;
}
