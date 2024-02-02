#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "spell_checker.h"

int main(int argc, char **argv)
{
    spell_checker_args args = load_args(argc, argv);
    rb_tree *dictionary = dictionary_create();
    dictionary_load_from_file(dictionary, args.dictionary_file_path);
    // TODO: add signal handling and make this loop end when SIGINT is received, dont terminate the whole program there as we want to cleanup the memory taken by dictionary
    while (1)
    {
        // main loop
    }
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
    int val = 0;
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
