#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "text_reader.h"

int read_words_from_file(const char *filename, char words[][MAX_WORD_LEN]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("No se pudo abrir el archivo");
        return 0;
    }

    char ch;
    char current_word[MAX_WORD_LEN];
    int word_index = 0, char_index = 0;

    while ((ch = fgetc(file)) != EOF) {
        if (isalnum(ch)) {
            current_word[char_index++] = ch;
        } else if (char_index > 0) {
            current_word[char_index] = '\0';
            strcpy(words[word_index++], current_word);
            char_index = 0;
        }
    }

    // Por si la última palabra no fue seguida por un separador
    if (char_index > 0) {
        current_word[char_index] = '\0';
        strcpy(words[word_index++], current_word);
    }

    fclose(file);
    return word_index;
}
