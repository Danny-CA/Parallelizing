#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 50
#define MAX_WORDS 10000
#define TEXT_SIZE 512

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
    
    char local_text[TEXT_SIZE] = {0};

    // Proceso 0 divide y envía los fragmentos de texto correctamente
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            int start = i * chunk_size;
            int end = (i == size - 1) ? text_len : start + chunk_size;

            while (start > 0 && isalpha(text[start])) start--;  // Asegurar palabra completa
            while (end < text_len && isalpha(text[end])) end++;  

            int len = end - start;
            MPI_Send(text + start, len, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
        strncpy(local_text, text, chunk_size);
    } else {
        MPI_Recv(local_text, TEXT_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    WordCount local_counts[MAX_WORDS];
    int local_word_count = 0;
    count_words(local_text, local_counts, &local_word_count);

    // Enviar los resultados al proceso 0
    if (rank != 0) {
        MPI_Send(local_counts, local_word_count * sizeof(WordCount), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    } else {
        WordCount global_counts[MAX_WORDS];
        int global_word_count = local_word_count;
        memcpy(global_counts, local_counts, local_word_count * sizeof(WordCount));

        for (int i = 1; i < size; i++) {
            WordCount temp_counts[MAX_WORDS];
            int temp_count;
            MPI_Recv(temp_counts, MAX_WORDS * sizeof(WordCount), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Fusionar los resultados
            for (int j = 0; j < MAX_WORDS; j++) {
                if (temp_counts[j].count > 0) {
                    int pos = find_word(global_counts, global_word_count, temp_counts[j].word);
                    if (pos == -1) {
                        strcpy(global_counts[global_word_count].word, temp_counts[j].word);
                        global_counts[global_word_count].count = temp_counts[j].count;
                        global_word_count++;
                    } else {
                        global_counts[pos].count += temp_counts[j].count;
                    }
                }
            }
        }

        // Imprimir los resultados
        printf("Word Counts:\n");
        for (int i = 0; i < global_word_count; i++) {
            printf("%s: %d\n", global_counts[i].word, global_counts[i].count);
        }
    }// Imprimir los resultados
        printf("Word Counts:\n");
        for (int i = 0; i < global_word_count; i++) {
            printf("%s: %d\n", global_counts[i].word, global_counts[i].count);
        }
    }

    MPI_Finalize();
    return 0;
}
