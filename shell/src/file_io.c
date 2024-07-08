#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "file_io.h"

void setup_input_redirections(const char* filename) {
    errno = 0;
    int input_fd = open(filename, O_RDONLY);
    if(input_fd == -1) {
        if(errno == EACCES)
            fprintf(stderr, "%s: permission denied\n", filename);
        else
            fprintf(stderr, "%s: no such file or directory\n", filename);
        exit(EXIT_FAILURE);
    }
    dup2(input_fd, STDIN_FILENO);
    close(input_fd);
}

void setup_output_redirections(const char* filename, int flags) {
    errno = 0;
    int output_fd = open(filename, O_WRONLY | O_CREAT | flags, 0644);
    if(output_fd == -1) {
        if(errno == EACCES) 
            fprintf(stderr, "%s: permission denied\n", filename); 
        else 
            fprintf(stderr, "%s: no such file or directory\n", filename);
        exit(EXIT_FAILURE);
    }
    dup2(output_fd, STDOUT_FILENO);
    close(output_fd);
}

void setup_redirections(command *com) {
    redirseq *first_redir = com->redirs;
    redirseq *redirs = com->redirs;

    if(redirs) {
        do {
            if(IS_RIN(redirs->r->flags))
                setup_input_redirections(redirs->r->filename);
            if (IS_RAPPEND(redirs->r->flags))
                setup_output_redirections(redirs->r->filename, O_APPEND);
            if(IS_ROUT(redirs->r->flags))
                setup_output_redirections(redirs->r->filename, O_TRUNC);
            redirs = redirs->next;
        } while (redirs != com->redirs);
    }
}