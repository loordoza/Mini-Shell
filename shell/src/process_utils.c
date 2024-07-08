#include <stdio.h>
#include <sys/wait.h>

#include "process_utils.h"
#include "signal_handlers.h"

void fg_init(fg_prc *fg) {
    fg->fg_cnt = 0;
}

void fg_insert(fg_prc *fg, pid_t pid) {
    fg->pids[fg->fg_cnt++] = pid;
}

int fg_member(fg_prc *fg, pid_t pid) {
    for (int i = 0; i < fg->fg_cnt; ++i)
        if (fg->pids[i] == pid)
            return 1;
    return 0;
}

void fg_delete(fg_prc *fg, pid_t pid) {
    for(int i=0; i<fg->fg_cnt; ++i) {
        if(fg->pids[i] == pid) {
            fg->pids[i] = fg->pids[fg->fg_cnt-1];
            fg->fg_cnt--;
            return;
        }
    }
}

void bg_init(bg_prc *bg) {
    bg->bg_cnt = 0;
}

void bg_make_note(bg_prc *bg, pid_t pid, int status) {
    bg->pids[bg->bg_cnt++] = pid;
    bg->signals[bg->bg_cnt-1] = status;
}

void bg_write_notes(bg_prc *bg) {
    block(SIGCHLD);
    for(int i=0; i<bg->bg_cnt; ++i) {
        if(WIFEXITED(bg->signals[i]))
            printf("Background process %d terminated. (exited with status %d)\n", bg->pids[i], WEXITSTATUS(bg->signals[i]));
        else if(WIFSIGNALED(bg->signals[i]))
            printf("Background process %d terminated. (killed by signal %d)\n", bg->pids[i], WTERMSIG(bg->signals[i]));
    }
    fflush(stdout);
    bg->bg_cnt = 0;
    unblock(SIGCHLD);
}