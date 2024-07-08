#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h> 

#include "config.h"
#include "builtins.h"
#include "file_io.h"
#include "process_utils.h"
#include "signal_handlers.h"

extern fg_prc fg_proc;
extern bg_prc bg_proc;

void fill_exec_argv(argseq *args, char *exec_argv[]) {
	int i=0;
	argseq *first_arg_ptr = args;
	for(argseq *arg = args; arg != NULL; arg = arg->next) {
		if(i != 0 && arg == first_arg_ptr)
			break;
		exec_argv[i] = arg->arg;
		i++;
	}
	exec_argv[i] = NULL;
}

int execute_builtins(command *com) {
	char *exec_argv[MAX_LINE_LENGTH / 2 + 1];
	if (com != NULL && com->args != NULL)
	for(int i=0; builtins_table[i].name != NULL; ++i) {
		if(strcmp(builtins_table[i].name, com->args->arg) == 0) {
			fill_exec_argv(com->args, exec_argv);
			if(builtins_table[i].fun(exec_argv) != 0)
				fprintf(stderr, "Builtin %s error.\n", builtins_table[i].name);
			return 1;
		}
	}
	return 0;
}

void execute_command(command *com, int in_bg) {
	char *exec_argv[MAX_LINE_LENGTH / 2 + 1];

	fill_exec_argv(com->args, exec_argv);
	setup_redirections(com);

	unblock(SIGCHLD);
	execvp(exec_argv[0], exec_argv);
	if(errno != 0) {
		if (errno == ENOENT)
			fprintf(stderr, "%s: no such file or directory\n", exec_argv[0]);
		else if (errno == EACCES)
			fprintf(stderr, "%s: permission denied\n", exec_argv[0]);
		else
			fprintf(stderr, "%s: exec error\n", exec_argv[0]);
		exit(EXEC_FAILURE);
	}
}

void execute_pipeline(pipeline *pl) {
	char *exec_argv[MAX_LINE_LENGTH / 2 + 1];
	int in_bg = 0;
	if(pl->flags == INBACKGROUND)
		in_bg = 1;

	commandseq *commands = pl->commands;

	if(commands == NULL)
		return;
	if(commands->next==commands && execute_builtins(commands->com))
		return;

	int prev_pipe[2] = {-1, -1};
	int pipe_fd[2];

	block(SIGCHLD);
	do {
		if(pipe(pipe_fd) == -1) {
			perror("pipe() failed");
			exit(EXIT_FAILURE);
		}

		pid_t pid = fork();

		if(pid == -1) {
			perror("fork() failed");
			exit(EXIT_FAILURE);
		}

		if(pid == 0) {
			if(in_bg)
				setsid();
			if(prev_pipe[0] != -1) {
				dup2(prev_pipe[0], STDIN_FILENO);
				close(prev_pipe[0]);
				close(prev_pipe[1]);
			}
			if(commands->next != pl->commands)
				dup2(pipe_fd[1], STDOUT_FILENO);
			close(pipe_fd[0]);
			close(pipe_fd[1]);

			execute_command(commands->com, in_bg);
			exit(EXIT_SUCCESS);
		} else {
			if(!in_bg)
				fg_insert(&fg_proc, pid);

			if(prev_pipe[0] != -1) {
				close(prev_pipe[0]);
				close(prev_pipe[1]);
			}
			prev_pipe[0] = pipe_fd[0];
			prev_pipe[1] = pipe_fd[1];
			commands = commands->next;
		}
	} while(commands != pl->commands);

	close(prev_pipe[0]);
	close(prev_pipe[1]);

	sigset_t emptyMask;
	sigemptyset(&emptyMask);
	while(fg_proc.fg_cnt > 0) 
		sigsuspend(&emptyMask);
	unblock(SIGCHLD);
}

void execute_line(char *input) {
	pipelineseq *ln;
	ln = parseline(input);
	if(ln == NULL)
		fprintf(stderr, "%s\n", SYNTAX_ERROR_STR);
	pipelineseq *ps = ln;
	do {
		execute_pipeline(ps->pipeline);
		ps = ps->next;
	} while(ps != ln);
}