#include <stdlib.h>

#include "dictionary.h"
#include "error_handler.h"

#define HINTS_LIMIT_MIN 1
#define HINTS_LIMIT_MAX 1024
#define DEFAULT_HINTS_LIMIT 10
#define DEFAULT_DICTIONARY_FILE_PATH "./assets/small_words.txt"

#define MIN(a, b) (a < b ? a : b)

typedef struct __spell_checker_args
{
    int hints_limit;            // followed by "-l" flag
    char *dictionary_file_path; // followed by "-p" flag
} spell_checker_args;

void usage(char *program_name);
spell_checker_args load_args(int argc, char **argv);
int get_words_distance(char *s1, char *s2);