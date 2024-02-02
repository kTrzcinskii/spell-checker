#include <errno.h>
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
