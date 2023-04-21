# CSCE3600GP2


**Compile using "make"**

**use make clean to clean up object files**
----------------------------------
CSCE3600 - Group Project 2

**Our team consists of four members:**

 

 - Kevin   
 -  Dewey
 -  Doug    
 - Abdul

**Organization of the Project**

We split up the work as followed

    Kevin was responsible for implementing the basic shell, cd, pwd, redirection, debugging, makefile
    Dewey was responsible for implementing PATH, piplining, exit and debugging some of the program
    Abdul was responsible for implementing myhistory and debugging the program
    Doug  was responsible for implementing the sigint control and myhistory
    
  Alias implementation and redirection were implemented by the entire group. We also worked together on defensive programming by ensuring the program was working correctly and compiling without error.

We utilized GitHub for our project. Kevin was unable to access the UNT VPN through his work laptop due to networking limitations, so we collaborated on GitHub. The project will still be uploaded to the UNT GitLab and submitted separately through Canvas.

**Design Overview**


NewShell is a simple shell program that allows users to execute basic commands, change directories, and handle input/output redirection. It also supports features such as command history, aliases, and setting the PATH environment variables.

Our shell program is designed to accept commands from the user via the command line interface. The program is structured to handle multiple commands separated by semicolons. When a command is executed, the shell creates a new process to handle the execution. The program utilizes process groups and signal handling to manage foreground and background processes. Each program is started in its own process group, with the process group ID equal to the PID of the process. When a process is in the foreground, it can receive signals from the terminal.

Our program also includes an alias command that allows the user to define a shortcut for commands. When an alias is defined, the program stores the alias in a list that can be accessed later. The user can list all existing aliases and remove aliases using the appropriate commands.


The program is structured as follows:

    main.c: the main driver program that parses user input, executes commands, and handles signals. 
    main.c also contains the following commands "cd" , "pwd", and "exit"
      
    
    myhistory.c: handles storing and retrieving command history.
    myhistory.h: contains all the header files required for the myhistory function
    
    Path.c: handles setting and getting the PATH environment variable.
    path.h: contains all the header files required for path function
    
    alias.c: handles setting and retrieving aliases.
    alias.c: contains all the header files required for alias function

**Complete Specifications**

MyShell handles input by parsing each line of input and breaking it down into individual commands using semicolons as delimiters. If there are no commands between semicolons, MyShell ignores the empty line.

Signal Handling - The program handles signals by using process groups to manage foreground and background processes. Each process is started in its own process group with the process group ID equal to the PID of the process. When a process is in the foreground, it can receive signals from the terminal.

Terminal Control - The program uses the "tcsetpgrp()" function to manage the foreground process group of the terminal. When a process is in the foreground, its process group is moved to the foreground of the terminal.



 MyShell also supports the following built-in commands:

    cd [path]: changes the current working directory to the specified path.
    pwd: prints the current working directory.
    exit: exits the shell program.
    myhistory: prints the command history up to a set of 20 (supports -c and -e)
    path [path1] [path2] ...: sets the PATH environment variable to the specified paths. ( + & - )
    alias: allows the user to setup alias for commands

Input/output redirection is supported by using < and > symbols to indicate input and output files.
 MyShell also supports pipelines by using the | symbol to separate commands.



**Known Bugs or Problems**


