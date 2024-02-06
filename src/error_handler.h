#ifndef ERROR_HANDLER
#define ERROR_HANDLER

#include <stdio.h>
#include <stdlib.h>

#define GENERAL_ERROR EXIT_FAILURE
#define NOT_IMPLEMENTED 2
#define FILE_ERROR 3

#define ERROR_MESSAGE_BUFFER 1024

#define ERR(source, error_code) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(error_code))

void handle_file_open_error(char *file_path);
void handle_file_close_error(char *file_path);
void handle_cannot_create_directory(char *directory_path);

#endif // ERROR_HANDLER