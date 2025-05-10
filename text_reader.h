#ifndef TEXT_READER_H
#define TEXT_READER_H

#define MAX_WORD_LEN 50

char (*read_words_from_file(const char *filename, int *word_count))[MAX_WORD_LEN];

#endif
