#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "process_utils.h"
#include "signal_handlers.h"
#include "user_input.h"
#include "execution_handler.h"

fg_prc fg_proc;
bg_prc bg_proc;
int isTerminal;

int main(int argc, char *argv[]) {
	struct stat stdin_stat;
	fg_init(&fg_proc);
	bg_init(&bg_proc);
	setup_signal_handlers();
	if(fstat(0, &stdin_stat) == -1) {
		perror("fstat() failed");
		exit(EXIT_FAILURE);
	}
	isTerminal = S_ISCHR(stdin_stat.st_mode);
	input_handler(isTerminal);
	return 0;
}