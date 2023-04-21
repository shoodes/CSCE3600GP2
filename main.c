#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include "myhistory.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "Path.h"
#include "alias.h"


#define MAX_LINE_LENGTH 512
#define MAX_ARGS 128

void init_path();
void sig_handler(int signal);

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

void parse_input(char *input, char *commands[], int *num_commands) {
    char *command = strtok(input, ";\n");
    *num_commands = 0;

    while (command != NULL) {
        commands[*num_commands] = command;
        (*num_commands)++;
        command = strtok(NULL, ";\n");
    }
}

void split_command(char *command, char **executable, char **args) {
    *executable = strtok(command, " \t");
    int i = 0;

    while (args[i++] = strtok(NULL, " \t")) {
        if (i >= MAX_ARGS) {
            break;
        }
    }
}

int has_redirect(char **args, char *symbol) {
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], symbol) == 0) {
            return i;
        }
    }
    return -1;
}

void remove_arg(char **args, int index) {
    for (int i = index; args[i]; i++) {
        args[i] = args[i + 1];
    }
}

void handle_redirection(char **args) {
    int input_index, output_index;

    if ((input_index = has_redirect(args, "<")) != -1) {
        int input_fd = open(args[input_index + 1], O_RDONLY);
        if (input_fd == -1) {
            perror("Failed to open input file");
            exit(1);
        }
        dup2(input_fd, STDIN_FILENO);
        close(input_fd);
        remove_arg(args, input_index);
        remove_arg(args, input_index);
    }

    if ((output_index = has_redirect(args, ">")) != -1) {
        int output_fd = open(args[output_index + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("Failed to open output file");
            exit(1);
        }
        dup2(output_fd, STDOUT_FILENO);
        close(output_fd);
        remove_arg(args, output_index);
        remove_arg(args, output_index);
    }
}

void execute_pipeline(char **args) {
    char *cmd1[MAX_ARGS];
    char *cmd2[MAX_ARGS];
    int i, pipe_index = has_redirect(args, "|");

    for (i = 0; i < pipe_index; i++) {
        cmd1[i] = args[i];
    }
    cmd1[i] = NULL;

    for (i = pipe_index + 1, pipe_index = 0; args[i]; i++, pipe_index++) {
        cmd2[pipe_index] = args[i];
    }
    cmd2[pipe_index] = NULL;

    int pipe_fd[2];
    pipe(pipe_fd);
    int pid1 = fork();

    if (pid1 == 0) {
    dup2(pipe_fd[1], STDOUT_FILENO);
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    execvp(cmd1[0], cmd1);
    perror("Failed to execute command 1");
    exit(1);
}

int pid2 = fork();

if (pid2 == 0) {
    dup2(pipe_fd[0], STDIN_FILENO);
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    execvp(cmd2[0], cmd2);
            perror("Failed to execute command 2");
    exit(1);
}

close(pipe_fd[0]);
close(pipe_fd[1]);
waitpid(pid1, NULL, 0);
waitpid(pid2, NULL, 0);
}

void exit_shell() {
    exit(0);
}
void execute_command(char *command) {
    pid_t child_pid;
    char *executable;
    char *args[MAX_ARGS];

    command = expand_alias(command);
    split_command(command, &executable, args);

    if (strcmp(executable, "cd") == 0) {
        change_directory(args[1]);
        add_to_history(command);
        return;
    } else if (strcmp(executable, "pwd") == 0) {
        print_working_directory();
        add_to_history(command);
        return;
    } else if (strcmp(executable, "exit") == 0) {
        exit_shell();
    } else if (strcmp(executable, "myhistory") == 0) {
        print_history();
        add_to_history(command);
    } else if (strcmp(executable, "path") == 0) {
        handle_path_command(args);
        add_to_history(command);
    } else if (strcmp(executable, "alias") == 0) {
        handle_alias_command(command);
        add_to_history(command);
        return;
    } else {
        add_to_history(command);
        child_pid = fork(); // process groups

        if (child_pid == 0) {
            setpgid(child_pid, child_pid); // process groups
            handle_redirection(args);

            if (has_redirect(args, "|") != -1) {
                execute_pipeline(args);
            } else {
                if (execvp(executable, args) == -1) {
                    perror("Failed to execute command");
                    _exit(1);
                }
            }
        } else if (child_pid > 0) {
            setpgid(child_pid, child_pid); // process groups
            tcsetpgrp(STDIN_FILENO, child_pid); // process groups
            waitpid(child_pid, NULL, 0);
            tcsetpgrp(STDIN_FILENO, getpid()); // process groups
        } else {
            perror("Failed to fork");
            exit(1);
        }
    }
}

void sig_handler(int signal) { //sig handler
    if (signal == SIGINT) {
        printf("\nSIGINT received. Ignoring.\n");
        fflush(stdout);
    } else if (signal == SIGTSTP) {
        printf("\nSIGTSTP received. Ignoring.\n");
        fflush(stdout);
    }
}


int main(int argc, char *argv[]) {
    // Add signal handling
    signal(SIGINT, sig_handler); //sig handler
    signal(SIGTSTP, sig_handler); // sig handler
    signal(SIGTTOU, SIG_IGN); // sig handler to ignore SIGTTOU

    init_path();

    char input[MAX_LINE_LENGTH];
    char *commands[MAX_ARGS];
    int num_commands;
    if (argc == 1) { // Interactive mode
    while (1) {
        printf("myshell> ");
        fgets(input, MAX_LINE_LENGTH, stdin);

        parse_input(input, commands, &num_commands);

        for (int i = 0; i < num_commands; i++) {
            execute_command(commands[i]);
        }
    }
} else if (argc == 2) { // Batch mode
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Failed to open file");
            exit(1);
        }

        while (fgets(input, MAX_LINE_LENGTH, file)) {
            printf("%s", input);

            expand_alias(input); // Expand aliases in input
            parse_input(input, commands, &num_commands);

            for (int i = 0; i < num_commands; i++) {
                execute_command(commands[i]);
            }
        }

        fclose(file);
    } else {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(1);
    }

    return 0;
}