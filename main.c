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
//sig handler init path and imports
void change_directory(char *path) { //cd
    if (path == NULL) { //if path null
        path = getenv("HOME"); // go home
    }

    if (chdir(path) != 0) { //chdir path is not 0
        perror("Failed to change directory"); //perror
    }
}

void print_working_directory() { //print working directory
    char cwd[PATH_MAX]; //current working directory max pa
    if (getcwd(cwd, sizeof(cwd)) != NULL) { //if not null
        printf("%s\n", cwd); // print cwd
    } else {// else
        perror("Failed to get current directory");//print error
    }
}

void parse_input(char *input, char *commands[], int *num_commands) { //parse input input, command, num commands,
    char *command = strtok(input, ";\n"); //srrtok input
    *num_commands = 0;//num commands zero

    while (command != NULL) { //loop
        commands[*num_commands] = command;//set commands to command
        (*num_commands)++; //incr num commands
        command = strtok(NULL, ";\n"); //set command new val
    }//end while
} //end parse

void split_command(char *command, char **executable, char **args) { //split
    *executable = strtok(command, " \t"); //execut eq strtok
    int i = 0; // i = 0

    while (args[i++] = strtok(NULL, " \t")) { // while
        if (i >= MAX_ARGS) { // if i> max args
            break; //end
        }
    }
}

int has_redirect(char **args, char *symbol) {
    for (int i = 0; args[i]; i++) { //args interate
        if (strcmp(args[i], symbol) == 0) { //strcmp args, symbol
            return i;//return i
        }
    }
    return -1; //return -1
}

void remove_arg(char **args, int index) { //remove arg
    for (int i = index; args[i]; i++) { //i index a rgs i plus plus
        args[i] = args[i + 1]; //args i = args i+1
    }
}

void handle_redirection(char **args) {
    int input_index, output_index; // Indexes for input and output redirection

    // Check for input redirection
    if ((input_index = has_redirect(args, "<")) != -1) { // Check if "<" is present in the args
        int input_fd = open(args[input_index + 1], O_RDONLY); // Open the file for reading
        if (input_fd == -1) { // If open fails, print error message and exit
            perror("Failed to open input file");
            exit(1);
        }
        dup2(input_fd, STDIN_FILENO); // Redirect standard input to the file descriptor
        close(input_fd); // Close the file descriptor
        remove_arg(args, input_index); // Remove the "<" argument from args
        remove_arg(args, input_index); // Remove the input file name from args
    }

    // Check for output redirection
    if ((output_index = has_redirect(args, ">")) != -1) { // Check if ">" is present in the args
        int output_fd = open(args[output_index + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open the file for writing, create if it doesn't exist
        if (output_fd == -1) { // If open fails, print error message and exit
            perror("Failed to open output file");
            exit(1);
        }
        dup2(output_fd, STDOUT_FILENO); // Redirect standard output to the file descriptor
        close(output_fd); // Close the file descriptor
        remove_arg(args, output_index); // Remove the ">" argument from args
        remove_arg(args, output_index); // Remove the output file name from args
    }
}

void execute_pipeline(char **args) {
    char *cmd1[MAX_ARGS]; //cmd1
    char *cmd2[MAX_ARGS];//cmd2
    int i, pipe_index = has_redirect(args, "|"); //i and pipe index

    for (i = 0; i < pipe_index; i++) { //loop
        cmd1[i] = args[i]; //cmd1
    }
    cmd1[i] = NULL; //set to null

    for (i = pipe_index + 1, pipe_index = 0; args[i]; i++, pipe_index++) { //loop
        cmd2[pipe_index] = args[i]; //cmd2
    } 
    cmd2[pipe_index] = NULL;//set to null

    int pipe_fd[2]; //pipe
    pipe(pipe_fd);
    int pid1 = fork(); //fork pid

    if (pid1 == 0) { //check pid
    dup2(pipe_fd[1], STDOUT_FILENO);
    close(pipe_fd[0]);//close pipe
    close(pipe_fd[1]);//close pipe
    execvp(cmd1[0], cmd1); //excvp
    perror("Failed to execute command 1");//error
    exit(1);//exit
}

int pid2 = fork(); //fork pid 2

if (pid2 == 0) { //check pid2
    dup2(pipe_fd[0], STDIN_FILENO); //dup2 pipe
    close(pipe_fd[0]); //close pipe
    close(pipe_fd[1]); //close pipe
    execvp(cmd2[0], cmd2);
            perror("Failed to execute command 2"); //error
    exit(1);
}

close(pipe_fd[0]); //close pipe
close(pipe_fd[1]); //close pipe
waitpid(pid1, NULL, 0); //waipid
waitpid(pid2, NULL, 0);//waitpid
}

void exit_shell() {
    exit(0); //exit 
}
void execute_command(char *command) {
    pid_t child_pid;
    char *executable;
    char *args[MAX_ARGS];

    command = expand_alias(command);
    split_command(command, &executable, args);

    // Handle built-in commands
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
        // Handle external commands
        add_to_history(command);
        child_pid = fork(); // create child process

        if (child_pid == 0) { // child process
            setpgid(child_pid, child_pid); // create process group for child
            handle_redirection(args);

            if (has_redirect(args, "|") != -1) { // if command has pipe
                execute_pipeline(args); // execute command with pipe
            } else {
                if (execvp(executable, args) == -1) { // execute command
                    perror("Failed to execute command");
                    _exit(1);
                }
            }
        } else if (child_pid > 0) { // parent process
            setpgid(child_pid, child_pid); // create process group for child
            tcsetpgrp(STDIN_FILENO, child_pid); // set child as foreground process
            waitpid(child_pid, NULL, 0); // wait for child to finish
            tcsetpgrp(STDIN_FILENO, getpid()); // set parent as foreground process
        } else { // fork failed
            perror("Failed to fork");
            exit(1);
        }
    }
}
void sig_handler(int signal) { //sig handler
    if (signal == SIGINT) {
        printf("\nSIGINT received. Ignoring.\n");//sig message
        fflush(stdout);
    } else if (signal == SIGTSTP) {
        printf("\nSIGTSTP received. Ignoring.\n"); //sig message
        fflush(stdout);//flush
    }
}

// Main function
int main(int argc, char *argv[]) {
// Add signal handling
signal(SIGINT, sig_handler); // Set up signal handler for SIGINT (Ctrl+C)
signal(SIGTSTP, sig_handler); // Set up signal handler for SIGTSTP (Ctrl+Z)
signal(SIGTTOU, SIG_IGN); // Ignore SIGTTOU signal
init_path(); // Initialize environment path

char input[MAX_LINE_LENGTH]; // Buffer to hold user input
char *commands[MAX_ARGS]; // Buffer to hold parsed commands
int num_commands; // Number of parsed commands

if (argc == 1) { // Interactive mode
    while (1) { // Loop until user exits
        printf("myshell> "); // Display prompt
        fgets(input, MAX_LINE_LENGTH, stdin); // Read user input

        parse_input(input, commands, &num_commands); // Parse input into commands

        for (int i = 0; i < num_commands; i++) { // Loop through each command and execute it
            execute_command(commands[i]);
        }
    }
} else if (argc == 2) { // Batch mode
    FILE *file = fopen(argv[1], "r"); // Open batch file for reading
    if (!file) { // Check if file opened successfully
        perror("Failed to open file"); // Display error message
        exit(1); // Exit with error code
    }

    while (fgets(input, MAX_LINE_LENGTH, file)) { // Read each line of the batch file
        printf("%s", input); // Print the current line

        expand_alias(input); // Expand any aliases in the current line
        parse_input(input, commands, &num_commands); // Parse the line into commands

        for (int i = 0; i < num_commands; i++) { // Loop through each command and execute it
            execute_command(commands[i]);
        }
    }

    fclose(file); // Close batch file
} else { // Invalid arguments
    fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]); // Display usage message
    exit(1); // Exit with error code
}

return 0; // Exit program with success code

}
