#ifndef ALIAS_H
#define ALIAS_H

#define MAX_ALIASES 50
#define MAX_ALIAS_NAME 50
#define MAX_ALIAS_VALUE 200

void handle_alias_command(char *command);
char *expand_alias(char *command);
char *get_alias_expansion(const char *alias_name);

#endif
