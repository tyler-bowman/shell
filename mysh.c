/*
 * Tyler Bowman
 *
 * CS441/541: Project 3
 *
 */
#include "mysh.h"

int main(int argc, char * argv[]) {
     int ret;
     jobs_list =  (Jobs*)malloc(sizeof(Jobs));
     history_list = (History*)malloc(sizeof(History));

     jobs_list -> count = 0;
     jobs_list -> head = NULL; //no sentinal node

     history_list -> count = 0;
     history_list -> head = NULL; //no sentinal node


    /*
     * Parse Command line arguments to check if this is an interactive or batch
     * mode run.
     */
    if( 0 != (ret = parse_args_main(argc, argv)) ) {
        fprintf(stderr, "Error: Invalid command line!\n");
        return -1;
    }

    /*
     * If in batch mode then process all batch files
     */
    if( TRUE == is_batch) {
        if( TRUE == is_debug ) {
            printf("Batch Mode!\n");
        }

        if( 0 != (ret = batch_mode(argv)) ) {
            fprintf(stderr, "Error: Batch mode returned a failure!\n");
        }
    }
    /*B
     * Otherwise proceed in interactive mode
     */
    else if( FALSE == is_batch ) {
        if( TRUE == is_debug ) {
            printf("Interactive Mode!\n");
        }

        if( 0 != (ret = interactive_mode()) ) {
            fprintf(stderr, "Error: Interactive mode returned a failure!\n");
        }
    }
    /*
     * This should never happen, but otherwise unknown mode
     */
    else {
        fprintf(stderr, "Error: Unknown execution mode!\n");
        return -1;
    }


    /*
     * Display counts
     */
    printf("-------------------------------\n");
    printf("Total number of jobs               = %d\n", total_jobs);
    printf("Total number of jobs in history    = %d\n", total_history);
    printf("Total number of jobs in background = %d\n", total_jobs_bg);

    /*
     * Cleanup
     */


    return 0;
}

int parse_args_main(int argc, char **argv)
{

	if(argc == 1) { // if no command line arguments were passed then this is an interactive mode run

		is_batch = FALSE; // ensure is_batch is false
		return 0;

	} else if (argc > 1) { // if command line arguments were supplied then this is batch mode

		is_batch = TRUE;

		num_batch_files = argc - 1;

		return 0;

	} else { // something unexpected happened

		return -1;

	}

}

int batch_mode(char * argv[])
{

    /*
     * For each file...
     */

	for(int i = 0; i < num_batch_files; i++) {

		FILE *file = fopen(argv[i + 1], "r"); // open the batch file

		if(!file) { // if there was an error then print a message and move on to the next file

			fprintf(stderr, "Error: Could not open file %s\n", argv[i + 1]);
			continue; // move on to next file

		}

		char *line = NULL;
		size_t len = 0;

		while (getline(&line, &len, file) != -1) { // read one line at a time

			if(parse_jobs(line) != 0) {

      		        	fprintf(stderr, "Error parsing jobs");

			}

		}

		free(line);
		fclose(file); // close the file

	}

    /*
     * Cleanup
     */


    return 0;
}

int interactive_mode(void)
{

	char *line = NULL;
	size_t len = 0;
				

    do {

        printf("%s", PROMPT); // print the prompt

        if(getline(&line, &len, stdin) == -1) { // read stdin, break out of loop if CTRL-D

		break;

	}

	if(parse_jobs(line) != 0) {
		fprintf(stderr, "Error parsing jobs");

	}

    } while( 1/* end condition */);

    builtin_wait(); // wait for background processes to finish

    /*
     * Cleanup
     */

    free(line);

    return 0;
}

/*
 * You will want one or more helper functions for parsing the commands
 * and then call the following functions to execute them
 */

void job_list_insert(job_t* job) {
  Program* p = (Program*)malloc(sizeof(Program));
  p->job = job;
  p->next = NULL;
  if (jobs_list->head == NULL) {
    jobs_list->head = p;
  } else {
    Program* current = jobs_list->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = p;
  }
  jobs_list->count++;
}

void history_list_insert(job_t* job) {
  Program* p = (Program*)malloc(sizeof(Program));
  p->job = job;
  p->next = NULL;
  if (history_list->head == NULL) {
    history_list->head = p;
  } else {
    Program* current = history_list->head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = p;
  }
  history_list->count++;
}

void remove_job(int pid) {
  if(jobs_list->count == 0){
    fprintf(stderr,"Error, Count is zero!\n");
    return;
  }
  Program* j = jobs_list->head;
  if(jobs_list->head->job->pid == pid) {
    jobs_list->head = jobs_list->head->next;
    free(j);
  } else {

    j = jobs_list->head;
    while(j->next != NULL && j->next->job->pid != pid){
      j = j->next;
    }

    if(j->next == NULL){
        fprintf(stderr,"Error, Job not in List\n");
        return;
    }
    Program* to_free = j->next;
    j->next = j->next->next;
    free(to_free);
  }
  jobs_list->count--;
}

int launch_job(job_t * loc_job)
{

    if(loc_job->argc == 0) { // conditional that stops a seg fault from occuring if command is null

	return 0;

    }

    history_list_insert(loc_job);
    total_history++;

    /*
     * Launch the job in either the foreground or background
     */
	if(strcmp(loc_job->binary, "jobs") == 0) {

		builtin_jobs();

	} else if (strcmp(loc_job->binary, "history") == 0) {

		builtin_history();

	} else if (strcmp(loc_job->binary, "wait") == 0) {

		builtin_wait();

	} else if (strcmp(loc_job->binary, "fg") == 0) {

		if(loc_job->argv[1] == NULL) { // no arg given

			builtin_fg();

		} else { // arg is given

			builtin_fg_num(atoi(loc_job->argv[1]));

		}

	} else if (strcmp(loc_job->binary, "exit") == 0) {

		builtin_exit(); // call built in exit which will display the counts
		exit(0); // exit with exit code 0

	} else {

	     total_jobs++; // increment total number of jobs
	     loc_job->job_num = total_jobs;

             if(loc_job->is_background) {
                 job_list_insert(loc_job);
             }

       	     // executables
             pid_t pid = fork();
             loc_job->pid = pid; // set loc_job's pid

             if (pid < 0) { // fork fails

                fprintf(stderr,"Fork failed\n");
                exit(1);

            } else if (pid == 0) {

       	        //implement redirection here
	    	if(loc_job->input_file != NULL) { // we want to redirect an input_file as the input rather than command line

	  		int file = open(loc_job->input_file, O_RDONLY);

			if(file < 0) {
                		 fprintf(stderr, "Failed to open input file\n");
		 		 return 0;
	      		}

	                if(dup2(file, STDIN_FILENO) == -1) { // redirect file input
		 		fprintf(stderr, "failed to redirect input\n");
		 		return 0;
	      		}

		      close(file);

	    	} else if (loc_job->output_file != NULL) { // we want to redirect the output into a file rather than printing to command line

			int file = open(loc_job->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

	      		if(file < 0) { // check to see if file is not opening
	         		fprintf(stderr, "Failed to open output file\n");
		 		return 0;
	      		}

              		if(dup2(file, STDOUT_FILENO) == -1) { //redirect file output
              	 		fprintf(stderr, "Failed to redirect output\n");
		 		return 0;
	      		}

	      		close(file); //this just closes the redirect not the file

	    	}

            	if(execvp(loc_job->binary, loc_job->argv) == -1) { // call execvp, if it returns a -1 then it failed

               		fprintf(stderr, "Invalid command\n");
                	exit(1);

		}

        } else if(!loc_job->is_background) { // the job is not in the background

	   	int status;
	   	waitpid(pid, &status, 0); // wait for job to finsh

	}

      }

    return 0;
}

int builtin_exit(void)
{

	builtin_wait(); // wait for all background jobs to finish

    	/*
    	 * Display counts
     	*/
    	printf("-------------------------------\n");
    	printf("Total number of jobs               = %d\n", total_jobs);
    	printf("Total number of jobs in history    = %d\n", total_history);
    	printf("Total number of jobs in background = %d\n", total_jobs_bg);

	free_history_list(); // free the history list
	free(jobs_list);

	return 0;

}

int builtin_jobs(void)
{
    Program* p = jobs_list->head;
    Program* temp;

    while(p != NULL) {

      int status;
      pid_t result = waitpid(p->job->pid, &status, WNOHANG);

      if(result == 0){ // job is still running

	 printf("[%d] Running	%s\n", p->job->job_num, p->job->full_command);
	 p = p->next;

      } else { // job is completed

	 printf("[%d] Done	%s\n", p->job->job_num, p->job->full_command);

	 //remove job from the jobs list after done is printed
	 temp = p;
	 p = p->next;
	 remove_job(temp->job->pid);

      }

    }
    return 0;
}

int builtin_history(void)
{
    Program* p = history_list->head;

    for(int i = 0; i < history_list->count; i++) { // for each program in history list

	printf("  %d	%s",(i + 1), p->job->full_command); // print that job

	if(p->job->is_background) { // if job was in background print & following the job

		printf(" &\n");

      	} else {

		 printf("\n");

      	}

      	p = p->next;

    }

    return 0;

}

int builtin_wait()
{

        Program* cur_program;

	while (jobs_list->count != 0) { // while the jobs list is not empty

		cur_program = jobs_list->head;

                int status;
		waitpid(cur_program->job->pid, &status, 0); // wait for job to complete

                remove_job(cur_program->job->pid); // remove that job from jobs list

	}

    return 0;
}

int builtin_fg(void)
{

	Program* cur_prog = jobs_list->head;
	Program* bring_fg = NULL;
	if(cur_prog == NULL) {
		fprintf(stderr, "No jobs to be brought to foreground\n");
		return -1;
	}

	while(cur_prog != NULL) { // iterate through jobs list
		if(cur_prog->job->is_background) { // if the job is in the background set it to bring_fg
			bring_fg = cur_prog;
		}
		cur_prog = cur_prog->next;
	}

	if(bring_fg == NULL) {

		fprintf(stderr, "No jobs to be brought to foreground\n");
                return -1;

	}

	// wait for job to complete
	int status;
	waitpid(bring_fg->job->pid, &status, 0);
	return 0;

}

int builtin_fg_num(int job_num)
{

        Program* cur_prog = jobs_list->head;
        Program* bring_fg = NULL;

        if(cur_prog == NULL) { // jobs list is empty
                fprintf(stderr, "No jobs to be brought to foreground\n");
                return -1;
        }

        while(cur_prog != NULL) { // iterate through jobs list
                if(cur_prog->job->job_num == job_num) { // if the job's pid matches the given num set it to bring_fg
                        bring_fg = cur_prog;
			break;
                }
                cur_prog = cur_prog->next;
        }

	if(bring_fg == NULL) {

                fprintf(stderr, "Job not found, either already completed or nonexistent\n");
                return -1;

        }

        // wait for job to complete
        int status;
        waitpid(bring_fg->job->pid, &status, 0);
        return 0;

}

/*
Function that trims the leading and trailing whitespace if there is any
*/
void trim_whitespace(char *command)
{

	char *start = command;

	while(isspace(*start)) { // remove the leading white space

               start++;

        }

	if (start != command) { //  if leading whitespace was trimmed

		memmove(command, start, strlen(start) + 1); // move the trailing chars

	}

	char *end = command + strlen(command) - 1;

	while (end > command && isspace(*end)) { // remove trailing whitespace

		end--;

	}

	*(end + 1) = '\0'; // null terminate

}

/*
Function that sets up an array holding the separators (&/;) in a line
*/
char *get_separators(char *line)
{

	char *separators = malloc(strlen(line) + 1);
	int sep_index = 0;

	for(int i = 0; i < strlen(line); i++) { // for each char in command

		if(line[i] == '&' || line[i] == ';') { // if that char is a seperator

			separators[sep_index] = line[i]; // add it to the seperators array
			sep_index++;

		}

	}

	separators[sep_index] = '\0'; // null terminate
	return separators;

}

/*
Large funciton that handles all the tokeniztion and setting up of each individual job struct
*/
int parse_jobs(char *line)
{

	line[strcspn(line, "\n")] = 0; // strip off new line

        char *separators = get_separators(line); // set up separators array
        char *command_save_ptr;
        char *command = strtok_r(line, ";&", &command_save_ptr); // split line on spaces and tabs
        int job_index = 0;

        while(command != NULL) { // execute command

		// set up job
                job_t* job = (job_t*)malloc(sizeof(job_t));
                trim_whitespace(command);
                job->full_command = strdup(command); // set job's full command to the command read in
                job->is_background = (separators[job_index] == '&');
                job->argc = 0;
                job->argv = malloc(MAX_COMMAND_LINE * sizeof(char *)); // allocate mem for argv
		job->pid = 0;
		job->job_num = 0;
		job->input_file = NULL;
		job->output_file = NULL;

		//increment total jobs ran in background if job.is_background is true
		if(job->is_background) {

			total_jobs_bg++;

		}

		if(job->argv == NULL) {

			free(job->full_command);
			return -1;

		}

                char *arg_save_ptr; // to save spot for strtok_r
                char *arg = strtok_r(command, " \t", &arg_save_ptr);

                while(arg != NULL) { // separate command by spaces and put in argv

			if(strcmp(arg, "<") == 0) { // next token is input file

				arg = strtok_r(NULL, " \t", &arg_save_ptr); // set arg to the file following <

				if(arg != NULL) {

					job->input_file = strdup(arg);

				}

			} else if (strcmp(arg, ">") == 0) { // next token is output file

				arg = strtok_r(NULL, " \t", &arg_save_ptr); // set arg to the file following >

                                if(arg != NULL) {

                                        job->output_file = strdup(arg);

                                }


			} else { // normal job

	                        job->argv[job->argc] = arg;
        	                job->argc++;

			}

                        arg = strtok_r(NULL, " \t", &arg_save_ptr);

                }

                job->argv[job->argc] = NULL; // null terminate argv
                job->binary = job->argv[0]; // set binary

//print out job's for testing
/*

                printf("job.full_command = %s\njob.argc = %d\njob.is_background = %d\njob.binary = %s\njob.pid = %d\njob.input_file = %s\njob.output_file = %s\n", job.full_command, job.argc, job.is_background, job.binary, job.pid, job.input_file, job.output_file);

                for(int i = 0; i < job.argc; i++) {

                        printf("%s\n", job.argv[i]);

                }

                printf("\n");

*/


                if(launch_job(job) != 0) {

                        //print an error message
			fprintf(stderr, "Error launching job\n");

                }

                command = strtok_r(NULL, ";&", &command_save_ptr); // set command to next command on that line
                job_index++;

        }

	free(separators); // free separators array

	return 0;

}

/*
Function that frees each part of job that had memory allocated
*/
void free_job(job_t *job) {

	free(job->argv);
	free(job->full_command);
	free(job->output_file);
	free(job->input_file);
	free(job);

}

/*
Function that frees the history list
*/
void free_history_list() {

	Program* cur_prog = history_list->head;
	Program* next_prog;

	while(cur_prog != NULL) {

		next_prog = cur_prog->next;
		free_job(cur_prog->job); // free job mem
		free(cur_prog); // free program
		cur_prog = next_prog;

	}

	//reset history list after freeing
	history_list->head = NULL;
	history_list->count = 0;

}
