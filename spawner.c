/******************************************************************************/
/*
 * program spawner
 */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "process.h"
#include "log.h"

static struct {
    char command[1024];
    char *args[1024];
    unsigned int concurrency;
} config = {
    .command = "",
    .concurrency = 4,
};
int keepgoing = 1;

static void print_help(const char *prg)
{
    fprintf(stdout, "%s [-h] [-c concurrency] {command line}\n", 
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

    process_init(config.concurrency);
    keepgoing = 1;
    while (keepgoing) {
        process_session(config.command, config.args);
    }

    exit(0);
}


