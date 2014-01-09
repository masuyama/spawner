#if !defined (__PROCESS_H_INCLUDED__)
#define __PROCESS_H_INCLUDED__

void process_init(unsigned int concurrency);
void process_release(void);

void process_session(const char *command, char *args[]);

void process_signal(int sig);

#endif  // !defined (__PROCESS_H_INCLUDED__)

