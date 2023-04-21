#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "alias.h"

typedef struct Alias {
    char *name;
    char *command;
    struct Alias *next;
} Alias;

static Alias *aliases = NULL;

static Alias *alias_create(char *name, char *command) {
    Alias *new_alias = (Alias *)malloc(sizeof(Alias));

    *new_alias = (Alias){
        .name = name,
        .command = command,
        .next = NULL
    };

    return new_alias;
}

static Alias *alias_destroy(Alias *alias_ptr) {
    if (alias_ptr) {
        alias_destroy(alias_ptr->next);
        free(alias_ptr->name);
        free(alias_ptr->command);
        free(alias_ptr);
    }

    return NULL;
}

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

static Alias *alias_add(Alias *alias_ptr, const char *name, const char *command) {
    alias_ptr = alias_remove(alias_ptr, name);

    Alias *new_alias = alias_create(strdup(name), strdup(command));

    new_alias->next = alias_ptr;
    return new_alias;
}

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
char *get_alias_expansion(const char *alias_name) {
    Alias *alias_ptr = aliases;
    while (alias_ptr) {
        if (strcmp(alias_name, alias_ptr->name) == 0) {
            return alias_ptr->command;
        }
        alias_ptr = alias_ptr->next;
    }
    return NULL;
}

char *expand_alias(char *command) {
    char *alias_name = strtok(command, " \t\n");
    char *alias_value = get_alias_expansion(alias_name);

    if (alias_value) {
    char *expanded_command = (char *)malloc(strlen(alias_value) + strlen(command) + 1);
    strcpy(expanded_command, alias_value);

    // If there are any remaining tokens, copy them to the expanded command.
    char *remaining_tokens = strtok(NULL, "\n");
    if (remaining_tokens) {
        strcat(expanded_command, " ");
        strcat(expanded_command, remaining_tokens);
    }

    free(command); // Free the original command memory
    return expanded_command;
}
return command;
}
