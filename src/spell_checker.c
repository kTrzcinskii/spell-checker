#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "spell_checker.h"

// TODO:
// - add option to specify file with words to check, instead of writting them manually
// TODO:
// - add option to write output to files, where for every word there will be created file with hints for given word (when word is correct then file is empty)

int main(int argc, char **argv)
{
    // load program options
    spell_checker_args args = load_args(argc, argv);

    // create dictionary and populate it with data from file
    rb_tree *dictionary = dictionary_create();
    dictionary_load_from_file(dictionary, args.dictionary_file_path);

    // main loop
    char *current_word;
    size_t current_word_length;
    while (1)
    {
        // TODO: add signal handling and make this loop end when C-D is received, dont terminate the whole program there as we want to cleanup the memory taken by dictionary
        while (getline(&current_word, &current_word_length, stdin) != -1)
        {
            // remove endline from the word
            current_word[strlen(current_word) - 1] = '\0';
            handle_word(dictionary, current_word, args.hints_limit);
        }
    }

    // free alocated memory
    free(current_word);
    dictionary_destroy(dictionary);
    return EXIT_SUCCESS;
}

void usage(char *program_name)
{
    fprintf(stderr, "USAGE: %s [-l hints_limits] [-p path]\n", program_name);
    fprintf(stderr, "hints_limits - maximum number of words returned as hints when provided word is not present in the dictionary [unsigned integer from range %d-%d, default - %d]\n", HINTS_LIMIT_MIN, HINTS_LIMIT_MAX, DEFAULT_HINTS_LIMIT);
    fprintf(stderr, "path - path to the text file with dictionary data [string, default - %s]", DEFAULT_DICTIONARY_FILE_PATH);
    exit(EXIT_FAILURE);
}

spell_checker_args load_args(int argc, char **argv)
{
    spell_checker_args args;

    // load args here
    // l -> hints limits
    // p -> dictionary file path
    char c;
    size_t val = 0;
    char *path = NULL;
    while ((c = getopt(argc, argv, "l:p:")) != -1)
    {
        switch (c)
        {
        case 'l':
            val = atoi(optarg);
            if (val < HINTS_LIMIT_MIN || val > HINTS_LIMIT_MAX)
                usage(argv[0]);
            break;
        case 'p':
            path = optarg;
            break;
        case '?':
            usage(argv[0]);
        }
    }

    if (argc > optind)
        usage(argv[0]);

    args.hints_limit = val != 0 ? val : DEFAULT_HINTS_LIMIT;
    args.dictionary_file_path = path != NULL ? path : DEFAULT_DICTIONARY_FILE_PATH;

    return args;
}

// FIXME: the function sometimes return incorrect values
int get_words_distance(char *s1, char *s2)
{
    // the result of this function is Levenshtein distance of two input words
    // if error occurs -1 is returned
    int n = strlen(s1) + 1;
    int m = strlen(s2) + 1;

    // allocate memory
    int **distance_array = malloc(sizeof(int *) * n);
    if (!distance_array)
        return -1;
    for (int i = 0; i < n; i++)
    {
        distance_array[i] = malloc(sizeof(int) * m);
        if (!distance_array[i])
        {
            for (int j = 0; j < i; j++)
                free(distance_array[j]);
            free(distance_array);
            return -1;
        }
    }

    // fill up base case scenario
    for (int i = 0; i < n; i++)
        distance_array[i][0] = i;
    for (int j = 0; j < m; j++)
        distance_array[0][j] = j;

    // calculate other distances
    for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++)
        {
            if (s1[i] == s2[j])
                distance_array[i][j] = distance_array[i - 1][j - 1];
            else
                distance_array[i][j] = MIN(MIN(distance_array[i - 1][j], distance_array[i][j - 1]), distance_array[i - 1][j - 1]) + 1;
        }

    // get final distance
    int distance = distance_array[n - 1][m - 1];

    // cleanup allocated memory
    for (int i = 0; i < n; i++)
        free(distance_array[i]);
    free(distance_array);

    return distance;
}

void handle_word(rb_tree *dictionary, char *word, int hints_limit)
{
    if (dictionary_search(dictionary, word, NULL))
        printf("%s is correct!\n", word);
    else
    {
        char **hints = NULL;
        size_t hints_size = 0;
        int min_distance = get_hints(dictionary, word, &hints, &hints_size, hints_limit);
        if (hints_size == 0)
        {
            printf("%s is not correct, couldn't find any close words in the dictionary\n", word);
        }
        else
        {
            printf("%s is not correct, here are %ld closest words from dictionary (with %d Levenshtein distance):\n", word, hints_size, min_distance);
            for (int i = 0; i < hints_size; i++)
                printf("- %s\n", hints[i]);
        }
        free(hints);
    }
}

bool is_word_in_dictionary(rb_tree *dictionary, char *word)
{
    if (dictionary_search(dictionary, word, NULL))
        return true;
    return false;
}

int get_hints(rb_tree *dictionary, char *word, char ***hints, size_t *hints_size, size_t hints_limit)
{
    int current_min_distance = __INT_MAX__;
    size_t current_hints_size = 0;
    *hints = malloc(sizeof(char *) * hints_limit);
    if (!*hints)
    {
        dictionary_destroy(dictionary);
        ERR("malloc", GENERAL_ERROR);
    }

    add_hint(dictionary->root, word, hints, &current_hints_size, &current_min_distance, hints_limit);

    *hints_size = current_hints_size;
    return current_min_distance;
}

void add_hint(rb_node *root, char *word, char ***hints, size_t *current_hints_size, int *current_min_distance, size_t hints_limit)
{
    if (!root)
        return;

    int dist = get_words_distance(word, root->word);

    //   remove current hints and add only this hint
    if (dist < *current_min_distance)
    {
        *current_min_distance = dist;
        for (int i = 0; i < (*current_hints_size); i++)
            (*hints)[i] = NULL;
        *current_hints_size = 0;
        (*hints)[(*current_hints_size)++] = root->word;
    }

    // add this hint to the list of other hints with same distance only if there is a space left
    else if (dist == *current_min_distance && *current_hints_size < hints_limit)
    {
        (*hints)[(*current_hints_size)++] = root->word;
    }

    // recursively search in left and right subtrees
    add_hint(root->left, word, hints, current_hints_size, current_min_distance, hints_limit);
    add_hint(root->right, word, hints, current_hints_size, current_min_distance, hints_limit);
}
