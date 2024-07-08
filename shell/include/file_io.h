#ifndef FILE_IO_H
#define FILE_IO_H

#include "siparse.h"

void setup_input_redirections(const char* filename);
void setup_output_redirections(const char* filename, int flags);
void setup_redirections(command *com);

#endif /* FILE_IO_H */