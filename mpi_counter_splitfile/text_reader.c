#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 50

const char *stopwords[] = {
    "a", "about", "above", "after", "again", "against", "all", "am", "an", "and",
    "any", "are", "aren't", "as", "at", "be", "because", "been", "before", "being",
    "below", "between", "both", "but", "by", "can't", "cannot", "could", "couldn't",
    "did", "didn't", "do", "does", "doesn't", "doing", "don't", "down", "during", 
    "each", "few", "for", "from", "further", "had", "hadn't", "has", "hasn't", 
    "have", "haven't", "having", "he", "he'd", "he'll", "he's", "her", "here", 
    "here's", "hers", "herself", "him", "himself", "his", "how", "how's", "i", 
    "i'd", "i'll", "i'm", "i've", "if", "in", "into", "is", "isn't", "it", "it's", 
    "its", "itself", "let's", "me", "more", "most", "mustn't", "my", "myself", 
    "no", "nor", "not", "of", "off", "on", "once", "only", "or", "other", "ought", 
    "our", "ours", "ourselves", "out", "over", "own", "same", "shan't", "she", 
    "she'd", "she'll", "she's", "should", "shouldn't", "so", "some", "such", "than", 
    "that", "that's", "the", "their", "theirs", "them", "themselves", "then", 
    "there", "there's", "these", "they", "they'd", "they'll", "they're", "they've", 
    "this", "those", "through", "to", "too", "under", "until", "up", "very", "was", 
    "wasn't", "we", "we'd", "we'll", "we're", "we've", "were", "weren't", "what", 
    "what's", "when", "when's", "where", "where's", "which", "while", "who", 
    "who's", "whom", "why", "why's", "with", "won't", "would", "wouldn't", "you", 
    "you'd", "you'll", "you're", "you've", "your", "yours", "yourself", "yourselves"
};
const int stopwords_count = sizeof(stopwords) / sizeof(stopwords[0]);

int is_stopword(const char *word) {
    for (int i = 0; i < stopwords_count; ++i)
        if (strcasecmp(word, stopwords[i]) == 0)
            return 1;
    return 0;
}
char (*read_words_from_file(const char *filename, int *word_count))[MAX_WORD_LEN] {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }
    int capacity = 1000;
    int word_index = 0;
    char (*words)[MAX_WORD_LEN] = malloc(capacity * MAX_WORD_LEN);
    if (!words) {
        perror("No se pudo asignar memoria inicial");
        fclose(file);
        return NULL;
    }
    char c;
    char current_word[MAX_WORD_LEN];
    int char_index = 0;

    while ((c = fgetc(file)) != EOF) {
        if (isalpha(c)) {
            if (char_index < MAX_WORD_LEN - 1) {
                current_word[char_index++] = tolower(c);
            }
        } else {
            if (char_index > 0) {
                current_word[char_index] = '\0';
                if (!is_stopword(current_word)) {
                    if (word_index >= capacity) {
                        capacity *= 2;
                        char (*temp)[MAX_WORD_LEN] = realloc(words, capacity * MAX_WORD_LEN);
                        if (!temp) {
                            perror("Error al expandir memoria");
                            free(words);
                            fclose(file);
                            return NULL;
                        }
                        words = temp;
                    }
                    strcpy(words[word_index++], current_word);
                }
                char_index = 0;
            }
        }
    }
    if (char_index > 0) {
        current_word[char_index] = '\0';
        if (!is_stopword(current_word)) {
            if (word_index >= capacity) {
                capacity *= 2;
                char (*temp)[MAX_WORD_LEN] = realloc(words, capacity * MAX_WORD_LEN);
                if (!temp) {
                    perror("Error al expandir memoria final");
                    free(words);
                    fclose(file);
                    return NULL;
                }
                words = temp;
            }
            strcpy(words[word_index++], current_word);
        }
    }
    fclose(file);
    *word_count = word_index;
    return words;
}
