#ifndef _USER_INPUT_H_
#define _USER_INPUT_H_

#include "siparse.h"

void fill_exec_argv(argseq*, char*[]);
int execute_builtins(command*);
void execute_command(command*, int);
void execute_pipeline(pipeline*);
void execute_line(char*);

#endif /* !_USER_INPUT_H_ */