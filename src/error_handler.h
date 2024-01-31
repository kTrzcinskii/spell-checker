#ifndef ERROR_HANDLER
#define ERROR_HANDLER

#include <stdio.h>

#define GENERAL_ERROR EXIT_FAILURE
#define NOT_IMPLEMENTED 2
#define FILE_ERROR 3

#define ERR(source, error_code) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(error_code))

#endif // ERROR_HANDLER