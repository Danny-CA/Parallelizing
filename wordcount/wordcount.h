#ifndef WORDCOUNT_H
#define WORDCOUNT_H

#include <stddef.h>

void parallel_word_count(const char *filename, int num_threads);

#endif
