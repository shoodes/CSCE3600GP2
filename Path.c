#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "Path.h"

static char paths[MAX_PATHS][MAX_PATH_LENGTH]; //max paths and max path lengths
static int num_paths; // num paths

void init_path() { //init path
    char *path = getenv("PATH"); //setting path
    if (path == NULL) { // if path is null
        printf("PATH FAIL"); // print path fail
        exit(1);//exit program
    }//end if

    char *path_copy = strdup(path); //path copy equals strdup path
    if (path_copy == NULL) { //path copy conditional 
        perror("Failed to allocate memory");//perror
        exit(1);//exit
    } //end if

    char *token = strtok(path_copy, ":"); //token =equals strtok path copy mem alloc

    while (token != NULL && num_paths < MAX_PATHS) { //run loop while path is still less than max path
        strcpy(paths[num_paths], token); //copy paths num paths , token
        token = strtok(NULL, ":");//set token
        num_paths++; //paths increment
    }// end while loop


    free(path_copy); //free up path copy
} // end init
void print_path() {  //print path
    printf("Path: "); //print

    for (int i = 0; i < num_paths; i++) { //running through path
        printf("Printing path %d\n", i); //print
        printf("%s", paths[i]); //print paths i
        if (i != num_paths - 1) { //value if i  not equals -1
            printf(":"); //print
        }// end if
    }
    printf("\n");//new line
}
void append_path(char *path) { //appends path
    if (num_paths == MAX_PATHS) { // if num path equals max
        printf("Max Paths reached\n"); //max paths
        return; //return
    }
    if (strlen(path) >= MAX_PATH_LENGTH) { //if len > max path length
        printf("Cannot add path, path too long\n"); //too  long
        return;//return
    }
    strncpy(paths[num_paths], path, MAX_PATH_LENGTH); //cpy num_paths, paths, max path lengtth
    paths[num_paths][MAX_PATH_LENGTH - 1] = '\0'; //null character
    num_paths++;//increment path
}


void remove_path(char *path) { //remove path
    int index = -1; //index -1
    for (int i = 0; i < num_paths; i++) { //iteriate num paths
        if (strcmp(paths[i], path) == 0) { //strcmp paths i. path equals 0
            index = i; //set index to i
            break;//break loop
        }//end if
    }// end for

    if (index == -1) { //if index equals -1
        printf("Cannot remove path, path not found\n"); // print not found
        return; // return
    }// end if

    for (int i = index; i < num_paths - 1; i++) { //loop
        strcpy(paths[i], paths[i + 1]); //cpy paths i, paths i+1
    }
    num_paths--; //num paths dec
}

char* get_full_path(char *command) {
    char *full_path = malloc(MAX_PATH_LENGTH + strlen(command) + 2); //malloc
    if (full_path == NULL) {//check if path null
        perror("Failed to allocate memory");//if goes wrong
        exit(1);//exit
    }

    for (int i = 0; i < num_paths; i++) { //iterate num paths
        sprintf(full_path, "%s/%s", paths[i], command); //print full path, format, paths i, command
        if (access(full_path, X_OK) == 0) { // access  full path/ exec
            return full_path; // return path
        }//end if
    }//end for

    free(full_path); //free mem
    return NULL; // return null
}//end get full path

void handle_path_command(char **args) {//start handle path
    if (args[1] == NULL) { //check args
        printf("No path\n");
        print_path();//print path 
    } else if (strcmp(args[1], "+") == 0) { // check if arg +
        printf("Adding path\n"); //add path
        if (args[2] == NULL) {//checl arg 2 null
            printf("Invalid path\n"); //invalid args
            return;// return
        }
        append_path(args[2]);//append path
    } else if (strcmp(args[1], "-") == 0) {//stringcmp condition w args 1
        printf("Removing path\n"); //print handle path
        if (args[2] == NULL) {//args 2 condition
            printf("Invalid path\n"); //printf
            return;//return
        }
        remove_path(args[2]); //remove path args 2
    } else {//else statement
        printf("Invalid path\n");//print
        return;//return
    }
}
