#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "alias.h"

typedef struct Alias {
    char *name;             // the name of the alias
    char *command;          // the command that the alias represents
    struct Alias *next;     // a pointer to the next alias in the linked list
} Alias;

static Alias *aliases = NULL;  // the head of the alias linked list

// Create a new alias struct with the given name and command.
static Alias *alias_create(char *name, char *command) {
    Alias *new_alias = (Alias *)malloc(sizeof(Alias));

    *new_alias = (Alias){
        .name = name,
        .command = command,
        .next = NULL
    };

    return new_alias;
}

// Recursively free all of the memory associated with the given alias struct.
static Alias *alias_destroy(Alias *alias_ptr) {
    if (alias_ptr) {
        alias_destroy(alias_ptr->next);
        free(alias_ptr->name);
        free(alias_ptr->command);
        free(alias_ptr);
    }

    return NULL;
}

// Remove the alias with the given name from the alias linked list.
static Alias *alias_remove(Alias *alias_ptr, const char *name) {
    if (alias_ptr == NULL) {
        return NULL;
    }

    if (strcmp(alias_ptr->name, name) == 0) {
        Alias *next = alias_ptr->next;
        free(alias_ptr->name);
        free(alias_ptr->command);
        free(alias_ptr);
        return next;
    }

    alias_ptr->next = alias_remove(alias_ptr->next, name);
    return alias_ptr;
}

// Add a new alias with the given name and command to the alias linked list.
// If an alias with the same name already exists, it will be replaced.
static Alias *alias_add(Alias *alias_ptr, const char *name, const char *command) {
    alias_ptr = alias_remove(alias_ptr, name);

    Alias *new_alias = alias_create(strdup(name), strdup(command));

    new_alias->next = alias_ptr;
    return new_alias;
}

// Handle an alias command. Depending on the specific command, it will add,
// remove, print, or clear the alias linked list.
void handle_alias_command(char *command) {
    if (strncmp(command, "alias -c", 8) == 0) {
        aliases = alias_destroy(aliases);
    } else if (strncmp(command, "alias -r ", 9) == 0) {
        aliases = alias_remove(aliases, command + 9);
    } else if (strncmp(command, "alias ", 6) == 0) {
        char *name = strtok(command + 6, "=");
        char *alias_command = strtok(NULL, "");
        if (alias_command && alias_command[0] == '\'') {
            alias_command++; // remove the leading single quote
            char *end_quote = strchr(alias_command, '\'');
            if (end_quote) *end_quote = '\0'; // remove the trailing single quote
            aliases = alias_add(aliases, name, alias_command);
        }
    } else {
        Alias *alias_ptr = aliases;
        if (alias_ptr) {
            while (alias_ptr) {
                printf("%s='%s'\n", alias_ptr->name, alias_ptr->command);
                alias_ptr = alias_ptr->next;
            }
        } else {
            printf("There are no set aliases.\n");
        }
    }
}
// This function takes an alias name as input and returns the corresponding command string
char *get_alias_expansion(const char *alias_name) {
    // Start at the beginning of the linked list of aliases
    Alias *alias_ptr = aliases;
    while (alias_ptr) { // Loop through the linked list of aliases
        // If the current alias name matches the input alias name, return the corresponding command string
        if (strcmp(alias_name, alias_ptr->name) == 0) {
            return alias_ptr->command;
        }
        // Move on to the next alias in the linked list
        alias_ptr = alias_ptr->next;
    }
    // If no matching alias was found, return NULL
    return NULL;
}

// This function takes a command string as input and expands any aliases it contains
char *expand_alias(char *command) {
    // Get the first token in the command string, which should be the name of an alias
    char *alias_name = strtok(command, " \t\n");
    // Use the alias name to look up the corresponding command string
    char *alias_value = get_alias_expansion(alias_name);

    // If the alias exists, replace the alias name with the corresponding command string
    if (alias_value) {
        // Allocate memory for the expanded command string, which is the size of the alias value plus the remaining command string
        char *expanded_command = (char *)malloc(strlen(alias_value) + strlen(command) + 1);
        // Copy the alias value into the expanded command string
        strcpy(expanded_command, alias_value);

        // If there are any remaining tokens in the command string, append them to the expanded command string
        char *remaining_tokens = strtok(NULL, "\n");
        if (remaining_tokens) {
            strcat(expanded_command, " ");
            strcat(expanded_command, remaining_tokens);
        }

        // Free the memory used by the original command string
        free(command);
        // Return the expanded command string
        return expanded_command;
    }
    // If the alias does not exist, return the original command string
    return command;
}
