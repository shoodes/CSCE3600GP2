#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myhistory.h"

static HistoryEntry history[MAX_HISTORY];
static int history_count = 0;

void add_to_history(const char *command) {
    if (history_count == MAX_HISTORY) {
        memmove(history, history + 1, sizeof(HistoryEntry) * (MAX_HISTORY - 1));
        history_count--;
    }
    strcpy(history[history_count].command, command);
    history_count++;
}

void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i].command);
    }
}

void clear_history() {
    history_count = 0;
}

void execute_history_entry(int index) {
    if (index < 1 || index > history_count) {
        printf("Invalid history index\n");
        return;
    }
    printf("Executing command: %s\n", history[index-1].command);
    system(history[index-1].command);
}
