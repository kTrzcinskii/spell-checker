#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "spell_checker.h"

int main(int argc, char **argv)
{
    // load program options
    spell_checker_args args = load_args(argc, argv);

    // create dictionary and populate it with data from file
    rb_tree *dictionary = dictionary_create();
    dictionary_load_from_file(dictionary, args.dictionary_file_path);

    char *current_word = NULL;
    size_t current_word_length = 0;

    // initalize input stream
    FILE *input_stream = initialize_input_stream(dictionary, args);

    // initalize output directory
    if (args.output_dir_path)
        if (initialize_output_directory(args.output_dir_path))
            handle_cannot_create_directory(args.output_dir_path);

    // main loop
    while ((getline(&current_word, &current_word_length, input_stream) != -1))
    {
        // remove endline from the word
        current_word[strlen(current_word) - 1] = '\0';
        handle_word(dictionary, current_word, args.hints_limit, args.output_dir_path);
    }

    // free alocated memory
    free(current_word);
    dictionary_destroy(dictionary);
    if (fclose(input_stream))
        handle_file_close_error(args.input_file_path);
    return EXIT_SUCCESS;
}

void usage(char *program_name)
{
    fprintf(stderr, "USAGE: %s [-l hints_limits] [-p path] [-i input]\n", program_name);
    fprintf(stderr, "hints_limits - maximum number of words returned as hints when provided word is not present in the dictionary [unsigned integer from range %d-%d, default - %d]\n", HINTS_LIMIT_MIN, HINTS_LIMIT_MAX, DEFAULT_HINTS_LIMIT);
    fprintf(stderr, "path - path to the text file with dictionary data [string, default - %s]\n", DEFAULT_DICTIONARY_FILE_PATH);
    fprintf(stderr, "input - path to the file with input words (each one in new line), when no input is provided then stdin is used as default input stream [string]\n");
    exit(EXIT_FAILURE);
}

spell_checker_args load_args(int argc, char **argv)
{
    spell_checker_args args;

    // load args here
    // l -> hints limits
    // p -> dictionary file path
    // i -> input file path
    // o -> output directory path
    char c;
    size_t val = 0;
    char *path = NULL;
    char *input = NULL;
    char *output = NULL;
    while ((c = getopt(argc, argv, "l:p:i:o:")) != -1)
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
        case 'i':
            input = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        case '?':
            usage(argv[0]);
        }
    }

    if (argc > optind)
        usage(argv[0]);

    args.hints_limit = val != 0 ? val : DEFAULT_HINTS_LIMIT;
    args.dictionary_file_path = path != NULL ? path : DEFAULT_DICTIONARY_FILE_PATH;
    args.input_file_path = input;
    args.output_dir_path = output;

    return args;
}

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
            if (s1[i - 1] == s2[j - 1])
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

void handle_word(rb_tree *dictionary, char *word, int hints_limit, char *output_dir_path)
{
    char *output_path = NULL;
    if (output_dir_path)
        output_path = combine_path(output_dir_path, word);

    FILE *output_stream = initialize_output_stream(dictionary, output_path);

    if (dictionary_search(dictionary, word, NULL))
        if (!output_path)
            fprintf(output_stream, "%s is correct\n", word);
        else
            fprintf(output_stream, "%d\n", 1);
    else
    {
        if (!output_path)
            fprintf(output_stream, "%s is not correct\n", word);
        else
            fprintf(output_stream, "%d\n", 0);

        char **hints = NULL;
        size_t hints_size = 0;
        int min_distance = get_hints(dictionary, word, &hints, &hints_size, hints_limit);

        if (output_path)
            fprintf(output_stream, "%ld\n", hints_size);

        if (hints_size > 0)
        {
            if (!output_path)
                fprintf(output_stream, "Here are %ld closest words from dictionary (with %d Levenshtein distance):\n", hints_size, min_distance);
            else
                fprintf(output_stream, "%d\n", min_distance);
            for (int i = 0; i < hints_size; i++)
                fprintf(output_stream, "%s\n", hints[i]);
        }
        free(hints);
    }

    if (output_path && fclose(output_stream))
        handle_file_close_error(output_path);
    free(output_path);
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

FILE *initialize_input_stream(rb_tree *dictionary, spell_checker_args args)
{
    if (!args.input_file_path)
        return DEFAULT_INPUT;

    FILE *input_stream = fopen(args.input_file_path, "r");
    if (!input_stream)
    {
        dictionary_destroy(dictionary);
        handle_file_open_error(args.input_file_path);
    }
    return input_stream;
}

FILE *initialize_output_stream(rb_tree *dictionary, char *output_path)
{
    if (!output_path)
        return DEFAULT_OUTPUT;

    FILE *output_stream = fopen(output_path, "w");
    if (!output_stream)
    {
        dictionary_destroy(dictionary);
        handle_file_open_error(output_path);
    }
    return output_stream;
}

int initialize_output_directory(char *output_dir_path)
{
    char dir_path[DIR_BUFFER] = {0};
    char *delim = "/";
    char *token = strtok(output_dir_path, delim);

    struct stat st = {0};
    int res;

    while (token != NULL)
    {
        if (token[0] == '.')
        {
            strcat(dir_path, token);
            token = strtok(NULL, delim);
            continue;
        }
        strcat(dir_path, delim);
        strcat(dir_path, token);
        // directory doesn't exist
        if (stat(dir_path, &st) == -1)
            if ((res = mkdir(dir_path, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
                return res;
        token = strtok(NULL, delim);
    }

    strcpy(output_dir_path, dir_path);
    return 0;
}

char *combine_path(char *p1, char *p2)
{
    char *path = malloc(sizeof(char) * (strlen(p1) + strlen(p2) + 2));
    strcpy(path, p1);
    path[strlen(p1)] = '/';
    path[strlen(p1) + 1] = '\0';
    strcat(path, p2);
    return path;
}
