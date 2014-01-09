/******************************************************************************/
/*
 * program spawner
 */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "process.h"
#include "log.h"

static struct {
    char command[1024];
    char *args[1024];
    unsigned int concurrency;
    unsigned char daemon_mode;
} config = {
    .command = "",
    .concurrency = 4,
    .daemon_mode = 0,
};
int keepgoing = 1;

static void catch_stop(int signo)
{
    print_log(LOG_INFO, "catch_stop : %d\n", signo);
    keepgoing = 0;
    process_signal(signo);
}

static void catch_hup(int signo)
{
}

static void set_signal(int signo, void (*func)(int signo))
{
    struct sigaction act;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, signo);
    act.sa_flags = 0;
    sigaction(signo, &act, NULL);
}

static void print_help(const char *prg)
{
    fprintf(stdout, "%s [-h] [-d] [-c concurrency] {command line}\n", 
                    prg);
}

static void parse_options(int argc, char *argv[]) 
{
    if (argc < 2) {
        // 少なくとも実行プログラムが引数として渡されてくる必要がある
        print_help(argv[0]);
        exit(0);
    }
    int i;
    for (i = 1 ; i < argc ; i++) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-c") == 0) {
                i++;
                config.concurrency = atoi(argv[i]);
            }
            else if (strcmp(argv[i], "-h") == 0) {
                print_help(argv[0]);
                exit(0);
            }
            else if (strcmp(argv[i], "-d") == 0) {
                config.daemon_mode = 1;
            }
            else {
                // unknown option
                print_log(LOG_ERR, "unknown option [%s]\n", argv[i]);
                exit(0);
            }
        }
        else {
            // option終了
            break;
        }
    }
    if (argc == i) {
        // 実行プログラムが引数として渡されてくる必要がある
        print_help(argv[0]);
        exit(0);
    }
    strncpy(config.command, argv[i++], sizeof(config.command)-1);
    int c = 0;
    config.args[c++] = config.command;
    for (; i < argc ; i++) {
        config.args[c++] = argv[i];
        if (c >= 1023) {
            break;
        }
    }
    config.args[c] = NULL;
}

int main(int argc, char *argv[])
{
    set_log_level(LOG_DEBUG);
    parse_options(argc, argv);

    set_signal(SIGINT, catch_stop);
    set_signal(SIGTERM, catch_stop);
    set_signal(SIGQUIT, catch_stop);
    set_signal(SIGHUP, catch_hup);

    if (config.daemon_mode == 1) {
        int nochdir = 1;
        int noclose = 0;
        if (daemon(nochdir, noclose) == -1) {
            print_log(LOG_ERR, "failed to launch daemon.\n");
            exit(-1);
        }
    }
    
    process_init(config.concurrency);
    keepgoing = 1;

    while (keepgoing) {
        process_session(config.command, config.args);
    }

    process_release();

    print_log(LOG_INFO, "spawner end\n");

    exit(0);
}


