#ifndef TEXT_READER_H
#define TEXT_READER_H

#define MAX_WORDS 1000
#define MAX_WORD_LEN 100

int read_words_from_file(const char *filename, char words[][MAX_WORD_LEN]);

#endif
