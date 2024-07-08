#ifndef PROCESS_UTILS_H
#define PROCESS_UTILS_H

#include <sys/types.h>

#define MAX_PROCESSES 1024

typedef struct {
	pid_t pids[MAX_PROCESSES];
	int fg_cnt;
} fg_prc;

typedef struct {
	pid_t pids[MAX_PROCESSES];
	int signals[MAX_PROCESSES];
	int bg_cnt;
} bg_prc;

void fg_init(fg_prc* fg);
void fg_insert(fg_prc* fg, pid_t pid);
int fg_member(fg_prc* fg, pid_t pid);
void fg_delete(fg_prc* fg, pid_t pid);

void bg_init(bg_prc* bg);
void bg_make_note(bg_prc* bg, pid_t pid, int status);
void bg_write_notes(bg_prc* bg);

#endif /* PROCESS_UTILS_H */