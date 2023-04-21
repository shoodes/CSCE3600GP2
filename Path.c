#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "Path.h"

static char paths[MAX_PATHS][MAX_PATH_LENGTH];
static int num_paths;

void init_path() {
    char *path = getenv("PATH");
    if (path == NULL) {
        printf("Failed to get PATH environment variable\n");
        exit(1);
    }

   // printf("Original PATH: %s\n", path); PATH DEBUGGING STATEMENTS

    char *path_copy = strdup(path);
    if (path_copy == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }

    //printf("PATH copy: %s\n", path_copy); PATH DEBUGGING STATEMENTS

    char *token = strtok(path_copy, ":");

    while (token != NULL && num_paths < MAX_PATHS) {
        //printf("Adding path: %s\n", token); PATH DEBUGGING STATEMENTS
        strcpy(paths[num_paths], token);
        token = strtok(NULL, ":");
        num_paths++;
    }

   // printf("Finished initializing paths\n"); PATH DEBUGGING STATEMENTS

    free(path_copy);
}
void print_path() {
    printf("In print_path\n");
    printf("Path: ");

    for (int i = 0; i < num_paths; i++) {
        printf("Printing path %d\n", i);
        printf("%s", paths[i]);
        if (i != num_paths - 1) {
            printf(":");
        }
    }
    printf("\n");
}
void append_path(char *path) {
    if (num_paths == MAX_PATHS) {
        printf("Cannot add path, maximum number of paths reached\n");
        return;
    }
    if (strlen(path) >= MAX_PATH_LENGTH) {
        printf("Cannot add path, path too long\n");
        return;
    }
    strncpy(paths[num_paths], path, MAX_PATH_LENGTH);
    paths[num_paths][MAX_PATH_LENGTH - 1] = '\0';
    num_paths++;
}


void remove_path(char *path) {
    int index = -1;
    for (int i = 0; i < num_paths; i++) {
        if (strcmp(paths[i], path) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Cannot remove path, path not found\n");
        return;
    }

    for (int i = index; i < num_paths - 1; i++) {
        strcpy(paths[i], paths[i + 1]);
    }
    num_paths--;
}

char* get_full_path(char *command) {
    char *full_path = malloc(MAX_PATH_LENGTH + strlen(command) + 2);
    if (full_path == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }

    for (int i = 0; i < num_paths; i++) {
        sprintf(full_path, "%s/%s", paths[i], command);
        if (access(full_path, X_OK) == 0) {
            return full_path;
        }
    }

    free(full_path);
    return NULL;
}

void handle_path_command(char **args) {
    //printf("handle_path_command: Handling path command\n"); PATH DEBUGGING STATEMENTS
    if (args[1] == NULL) {
        printf("handle_path_command: No arguments\n");
        print_path();
    } else if (strcmp(args[1], "+") == 0) {
        printf("handle_path_command: Adding path\n");
        if (args[2] == NULL) {
            printf("Invalid arguments\n");
            return;
        }
        append_path(args[2]);
    } else if (strcmp(args[1], "-") == 0) {
        printf("handle_path_command: Removing path\n");
        if (args[2] == NULL) {
            printf("Invalid arguments\n");
            return;
        }
        remove_path(args[2]);
    } else {
        printf("Invalid arguments\n");
        return;
    }
}