#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myhistory.h"

// Define a fixed-size array to hold the history of executed commands
static HistoryEntry history[MAX_HISTORY];
// Keep track of the number of entries currently in the history array
static int history_count = 0;

// Add a new command to the history array
void add_to_history(const char *command) {
    // If the history array is full, shift all entries one position to the left
    if (history_count == MAX_HISTORY) {
        memmove(history, history + 1, sizeof(HistoryEntry) * (MAX_HISTORY - 1));
        history_count--;
    }
    // Copy the new command into the next available entry in the history array
    strcpy(history[history_count].command, command);
    // Increment the history count to reflect the new entry
    history_count++;
}

// Print the full history of executed commands
void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i].command);
    }
}

// Clear the history by resetting the history count to zero
void clear_history() {
    history_count = 0;
}

// Execute a specific command from the history by its index
void execute_history_entry(int index) {
    // If the index is out of range, print an error message and return
    if (index < 1 || index > history_count) {
        printf("Invalid history index\n");
        return;
    }
    // Print the command that is about to be executed
    printf("Executing command: %s\n", history[index-1].command);
    // Call the system function to execute the command
    system(history[index-1].command);
}
