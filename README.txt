# CS441/541 Shell Project

## Author(s):

Tyler Bowman


## Date:

10/7/2024


## Description:

The C program mysh.c is a program that implements a simple Unix shell. The shell operates as follows: when the user types in a command, the shell creates a child process that executes that command entered and then prompts for more user input when it has finished. From a simplicity standpoint, the shell runs in a while loop, which repeatedly asks for input to tell it what command to execute. This implementation of the shell has two modes: interactive mode and batch mode. Interactive mode is what you think of when you think of using a shell. It basically runs a while loop where it takes commands following the prompt "mysh$" and executes those given commands. Batch mode performs similarly, but instead of commands being typed into the command line, the commands are read in from a provided file. In terms of the commands the shell supports, we implemented the following: 'jobs', 'history', 'wait', 'fg', and 'exit'. Any other commands entered are treated as an executable. The 'jobs' command displays a list of jobs that are currently in the background. The 'history' command displays all of the jobs executed by the shell. The 'wait' command waits for all currently backgrounded jobs to complete. The 'fg' command waits for a specific currently backgrounded job to complete, in other words, it brings a specific currently backgrounded job to the foreground. Finally, the 'exit' command prints a count of total number of jobs executed, the total number of jobs in history, and the total number of jobs executed in the background. It then terminates the shell. Altogether, this was a pretty rigorous project, but it definitely gave me a greater appreciation of the shells I use every day.


## How to build the software

The included Makefile is used to build (compile and link) this software. It can be used as follows:
 - To build the software, run "make" from the command line.
 - To build the software and run the given tests, run "make check" from the command line.
 - To clean the directory, run "make clean" from the command line.


## How to use the software

To use the shell, you can either type commands interactively or provide a batch file contianing commands. To enter interactive mode, enter the command ./mysh. If you want to use batch mode, enter the command ./mysh followed by the batch file (./mysh tests/level01.txt). In interactive mode, simply type your commands at the prompt. The shell supports built-in commands like 'jobs', 'history', 'fg', 'wait', and 'exit', as well as executing external programs. You can run commands sequentially by separating them with ';' (date ; pwd ; whoami). Or, you can run them in the background, separating them with  '&' (sleep 5 & sleep 10 & date). If running just a single command in the background, follow it with an '&' as well (sleep 10 &). For redirection, use '<' to read from a file as input and '>' to redirect output to a file. When you're ready to exit the shell, use the 'exit' command. 

## How the software was tested

A project of this magnitude required a ton of incremental testing. Throughout the process, I used a ton of print statements as well as gdb to ensure the software was behaving as intended. I also read through the project description more times than I'd like to admit. In terms of actual processes running correctly, I started by using fork and exec to allow any executables to run. Once I was able to get this working, I began to work on the built-in commands. Once I felt I had the shell working as intended, I read through the document once more, completing each of the tests displayed in the document. Finally, I looked through the make check to see that it was passing all of the job counts and that the output was what it was expected to be. Altogether, testing this shell incrementally was key to actually completing this project. Without being intentional with our testing, I would've never completed this project. 


## Known bugs and problem areas

There are no known bugs or problem areas.
