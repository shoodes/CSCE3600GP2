#ifndef MYHISTORY_H
#define MYHISTORY_H

#define MAX_HISTORY 20

typedef struct {
    char command[100];
} HistoryEntry;

void add_to_history(const char *command);
void print_history();
void clear_history();
void execute_history_entry(int index);

#endif
