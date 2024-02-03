#include <stdbool.h>
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
void handle_word(rb_tree *dictionary, char *word, int hints_limit);
bool is_word_in_dictionary(rb_tree *dictionary, char *word);
int get_hints(rb_tree *dictionary, char *word, char ***hints, size_t *hints_size, size_t hints_limit);
void add_hint(rb_node *root, char *word, char ***hints, size_t *current_hints_size, int *current_min_distance, size_t hints_limit);