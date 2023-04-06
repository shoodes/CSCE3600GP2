#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// KEVIN "CD" 
void change_directory(char *path) {
    if (path == NULL) {
        path = getenv("HOME");
    }

    if (chdir(path) != 0) {
        perror("Failed to change directory");
    }
}

void print_working_directory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("Failed to get current directory");
    }
}
// ADD YOUR STUFF HERE

