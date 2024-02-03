#include "error_handler.h"

void handle_file_open_error(char *file_path)
{
    char buffer[ERROR_MESSAGE_BUFFER] = {0};
    snprintf(buffer, ERROR_MESSAGE_BUFFER, "Couldn't open the file (%s)", file_path);
    ERR(buffer, FILE_ERROR);
}

void handle_file_close_error(char *file_path)
{
    char buffer[ERROR_MESSAGE_BUFFER] = {0};
    snprintf(buffer, ERROR_MESSAGE_BUFFER, "Couldn't close the file (%s)", file_path);
    ERR(buffer, FILE_ERROR);
}
