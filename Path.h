#include <unistd.h>
#ifndef PATH_H
#define PATH_H

#define MAX_PATHS 10
#define MAX_PATH_LENGTH 256

void handle_path_command(char **args);
void init_path();
void print_path();
void append_path(char *path);
void remove_path(char *path);
char* get_full_path(char *command);

#endif
