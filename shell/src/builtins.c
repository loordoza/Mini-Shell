#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include "builtins.h"

int lexit(char*[]);
int echo(char*[]);
int lcd(char*[]);
int lkill(char*[]);
int lls(char*[]);
int undefined(char*[]);

builtin_pair builtins_table[]={
	{"exit",	&lexit},
	{"lecho",	&echo},
	{"lcd",		&lcd},
	{"cd",		&lcd},
	{"lkill",	&lkill},
	{"lls",		&lls},
	{NULL,NULL}
};

int 
lexit(char* argv[])
{
	exit(EXIT_SUCCESS);
}

int 
echo( char * argv[])
{
	int i = 1;
	if (argv[i]) printf("%s", argv[i++]);
	while  (argv[i])
		printf(" %s", argv[i++]);
	printf("\n");
	fflush(stdout);
	return 0;
}

int 
lcd(char* argv[])
{
	if(argv[2] != NULL)
		return BUILTIN_ERROR;
	if(argv[1] == NULL) {
		if(chdir(getenv("HOME")) < 0)
			return BUILTIN_ERROR;
	} else {
		if(chdir(argv[1]) < 0)
			return BUILTIN_ERROR;
	}
	return EXIT_SUCCESS;
}

int 
lkill(char* argv[])
{
	int signal_number = SIGTERM;
	int pid;
	char *endptr;

	argv++;
	if(argv[0] == NULL)
		return BUILTIN_ERROR;
	if(argv[0][0] == '-') {
		errno = 0;
		long result = strtol(argv[0]+1, &endptr, 10);
		if(*endptr != '\0')
			return BUILTIN_ERROR;
		if(errno != 0)
			return BUILTIN_ERROR;
		if(result <= 0 || result > INT_MAX)
			return BUILTIN_ERROR;
		signal_number = result;
		argv++;
	}
	if(argv[0] == NULL)
		return BUILTIN_ERROR;

	errno = 0;
	long result = strtol(argv[0], &endptr, 10);
	if(*endptr != '\0')
		return BUILTIN_ERROR;
	if(errno != 0)
		return BUILTIN_ERROR;
	if(result <= 0 || result > INT_MAX)
		return BUILTIN_ERROR;
	pid = result;
		
	if(kill(pid,signal_number) < 0)
		return BUILTIN_ERROR;
	return EXIT_SUCCESS;
}

int 
lls(char* argv[])
{
	DIR* dir;
	struct dirent* entry;
	dir = opendir(".");
	if(dir == NULL)
		return BUILTIN_ERROR;
	errno = 0;
	while((entry = readdir(dir)) != NULL) {
		if(entry->d_name[0] != '.') {
			printf("%s\n", entry->d_name);
		}
	}
	if(errno != 0)
		return BUILTIN_ERROR;
	fflush(stdout);
	if(closedir(dir) < 0)
		return BUILTIN_ERROR;
	return EXIT_SUCCESS;
}

int 
undefined(char * argv[])
{
	fprintf(stderr, "Command %s undefined.\n", argv[0]);
	return BUILTIN_ERROR;
}
