#include <stdbool.h>
#include <stdlib.h>

#include "dictionary.h"
#include "error_handler.h"

#define HINTS_LIMIT_MIN 1
#define HINTS_LIMIT_MAX 1024
#define DEFAULT_HINTS_LIMIT 10
#define DEFAULT_DICTIONARY_FILE_PATH "./assets/small_words.txt"
#define DEFAULT_INPUT stdin
#define DEFAULT_OUTPUT stdout

#define DIR_BUFFER 512

#define MIN(a, b) (a < b ? a : b)

typedef struct __spell_checker_args
{
    int hints_limit;            // following "-l" flag
    char *dictionary_file_path; // following "-p" flag
    char *input_file_path;      // following "-i" flag
    char *output_dir_path;      // following "-o" flag
} spell_checker_args;

void usage(char *program_name);
spell_checker_args load_args(int argc, char **argv);
int get_words_distance(char *s1, char *s2);
void handle_word(rb_tree *dictionary, char *word, int hints_limit, char *output_dir_path);
bool is_word_in_dictionary(rb_tree *dictionary, char *word);
int get_hints(rb_tree *dictionary, char *word, char ***hints, size_t *hints_size, size_t hints_limit);
void add_hint(rb_node *root, char *word, char ***hints, size_t *current_hints_size, int *current_min_distance, size_t hints_limit);
FILE *initialize_input_stream(rb_tree *dictionary, spell_checker_args args);
FILE *initialize_output_stream(rb_tree *dictionary, char *output_path);
int initialize_output_directory(char *output_dir_path);
char *combine_path(char *p1, char *p2);