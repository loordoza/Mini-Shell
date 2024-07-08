#ifndef _SIGNAL_HANDLERS_H_
#define _SIGNAL_HANDLERS_H_

void sigchld_handler();
void sigint_handler();
void block(int);
void unblock(int);
void setup_signal_handlers();

#endif /* !_SIGNAL_HANDLERS_H_ */
