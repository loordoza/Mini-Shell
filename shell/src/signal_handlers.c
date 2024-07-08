#include <stdio.h>
#include <stdlib.h>
#include <signal.h> 
#include <unistd.h>
#include <errno.h>

#include "signal_handlers.h"
#include "process_utils.h"

extern fg_prc fg_proc;
extern bg_prc bg_proc;
extern int isTerminal;

void sigchld_handler() {
	int olderrno = errno;
	while(1) {
		int status;
		pid_t pid = waitpid(-1, &status, WNOHANG);
		if(pid <= 0)
			break;
		if(fg_member(&fg_proc, pid))
			fg_delete(&fg_proc, pid);
		else {
			bg_make_note(&bg_proc, pid, status);
		}
	}
	errno = olderrno;
}

void sigint_handler() {
	int olderrno = errno;
	if(isTerminal) {
		if(write(STDOUT_FILENO, "\n", 1) == -1) {
			perror("write() failed");
			_exit(EXIT_FAILURE);
		}
	}
	errno = olderrno;
}

void block(int signal) {
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, signal);
	if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
		perror("sigprocmask() failed");
		exit(EXIT_FAILURE);
	}
}

void unblock(int signal) {
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, signal);
	if(sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
		perror("sigprocmask() failed");
		exit(EXIT_FAILURE);
	}
}

void setup_signal_handlers() {
	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	if(sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}

	struct sigaction sa2;
	sa2.sa_handler = sigint_handler;
	sigemptyset(&sa2.sa_mask);
	if(sigaction(SIGINT, &sa2, NULL) == -1) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}
}