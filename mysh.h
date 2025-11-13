/*
 * Tyler Bowman
 *
 * CS441/541: Project 3
 *
 */
#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "stdbool.h"
#include "fcntl.h"

/* For fork, exec, sleep */
#include <sys/types.h>
#include <unistd.h>
/* For waitpid */
#include <sys/wait.h>

/******************************
 * Defines
 ******************************/
#define TRUE  1
#define FALSE 0

#define MAX_COMMAND_LINE 1024

#define PROMPT ("mysh$ ")


/******************************
 * Structures
 ******************************/
/*
 * A job struct.  Feel free to change as needed.
 */
struct job_t {
    char * full_command;
    int argc;
    char **argv;
    int is_background;
    char * binary;
    pid_t pid;
    char *input_file;
    char *output_file;
    int job_num;
};
typedef struct job_t job_t;

/*
 * A program struct that is used in the job and history lists
 */
typedef struct Program {
    job_t* job;
    struct Program* next;
} Program;

/*
 * Jobs linked list struct
 */
typedef struct Jobs {
    int count;
    struct Program* head;
} Jobs;

/*
 * History linked list struct
 */
typedef struct History {
    int count;
    struct Program* head;
} History;

/******************************
 * Global Variables
 ******************************/

/*
 * Interactive or batch mode
 */
int is_batch = FALSE;

/*
 * Counts
 */
int total_jobs_display_ctr = 0;
int total_jobs    = 0;
int total_jobs_bg = 0;
int total_history = 0;
int num_batch_files = 0;

/*
 * Debugging mode
 */
int is_debug = FALSE;

/*
 * Job and History lists as global vairables
 */
Jobs* jobs_list;
History* history_list;


/******************************
 * Function declarations
 ******************************/
/*
 * Parse command line arguments passed to myshell upon startup.
 *
 * Parameters:
 *  argc : Number of command line arguments
 *  argv : Array of pointers to strings
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int parse_args_main(int argc, char **argv);

/*
 * Main routine for batch mode
 *
 * Parameters:
 *  None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int batch_mode(char * argv[]);

/*
 * Main routine for interactive mode
 *
 * Parameters:
 *  None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int interactive_mode(void);

/*
 * Launch a job
 *
 * Parameters:
 *   loc_job : job to execute
 *
 * Returns:
 *   0 on success
 *   Negative value on error 
 */
int launch_job(job_t * loc_job);

/*
 * Built-in 'exit' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_exit(void);

/*
 * Built-in 'jobs' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_jobs(void);

/*
 * Built-in 'history' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_history(void);

/*
 * Built-in 'wait' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_wait();

/*
 * Built-in 'fg' command
 *
 * Parameters:
 *   None (use default behavior)
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_fg();

/*
 * Built-in 'fg' command
 *
 * Parameters:
 *   Job id
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_fg_num(int job_num);

/*
 * Function that trims the whitespace off the beginning and end of a string
 *
 * Parameters:
 *   a command string
 *
*/
void trim_whitespace(char *command);


/*
 * Function that sets up an array holding the separators (&/;) in a line
 *
 * Parameters:
 *   a string of all commands on a single line
 *
 * Returns:
 *   an array containing separators (&/;) in the order they appear
 */
char *get_separators(char *line);

/*
 * Large function that handles all the tokenization and setting up of each individual job struct
 *
 * Parameters:
 *   a string of all commands on a single line
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int parse_jobs(char *line);

/*
 * Frees each part of Job that has memory allocated
 *
 * Parameters:
 *   a job struct
 */
void free_job(job_t *job);

/*
 * Frees history list while also calling free jobs to free each job in the history list
 */
void free_history_list();

/*
 * Function that adds a job to the job list
 *
 * Parameters:
 *   a job struct
 *
 */
void job_list_insert(job_t* job);

/*
 * Function that adds a job to the history list
 *
 * Parameters:
 *   a job struct
 */
void history_list_insert(job_t* job);

/*
 * Function that removes a job from the job list
 *
 * Parameters:
 *   the pid of a specific job
 */
void remove_job(int pid);

#endif /* MYSHELL_H */
