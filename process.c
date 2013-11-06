#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include "process.h"
#include "log.h"

typedef struct _spawner_process {
    pid_t   pid;
} spawner_process_t;

spawner_process_t   *process_list = NULL;
unsigned int        num_process = 0;
unsigned int        running_process = 0;

static spawner_process_t * find_free_slot(void)
{
    int i;
    spawner_process_t *p = process_list;
    for (i = 0 ; i < num_process ; i++, p++) {
        if (p->pid == -1) {
            return p;
        }
    }
    return NULL;
}

static spawner_process_t * find_slot_by_pid(pid_t pid)
{
    int i;
    spawner_process_t *p = process_list;
    for (i = 0 ; i < num_process ; i++, p++) {
        if (p->pid == pid) {
            return p;
        }
    }
    return NULL;
}

static void catch_sigchld(int signo)
{
    int pid, status;

    pid = waitpid(-1, &status, WNOHANG);
    if (pid < 0) {
        // error
        print_log(LOG_ERR, "%s : waitpid failed : %u\n", __func__, errno);
    }
    else if (pid == 0) {
        // 状態変化が起こっていない
        print_log(LOG_ERR, "%s : waitpid pid == 0\n", __func__);
    }
    else {
        spawner_process_t *p = find_slot_by_pid(pid);
        if (p == NULL) {
            print_log(LOG_ERR, "%s : pid not found : %u\n", __func__, pid);
        }
        else {
            p->pid = -1;
            running_process--;
            print_log(LOG_INFO, "process fin : %u\n", pid);
        }
    }
}

void process_init(unsigned int concurrency)
{
    if (concurrency == 0) {
        print_log(LOG_WARN, "%s : concurrency is 0\n", __func__);
        return;
    }
    process_list = (spawner_process_t *)calloc(concurrency, sizeof(spawner_process_t));
    if (process_list == NULL) {
        print_log(LOG_WARN, "%s : can not alloc process_list : %u\n", __func__, concurrency);
        return;
    }
    num_process = concurrency;
    int i;
    spawner_process_t *p = process_list;
    for (i = 0 ; i < num_process ; i++, p++) {
        p->pid = -1;
    }

    signal(SIGCHLD, catch_sigchld);

    print_log(LOG_DEBUG, "%s : success : %u, %p\n", __func__, num_process, process_list);
}

void process_release(void)
{
    if (process_list != NULL) {
        free(process_list);
        process_list = NULL;
    }
    num_process = 0;
    print_log(LOG_DEBUG, "%s : success : %u, %p\n", __func__, num_process, process_list);
}

void process_session(const char *command, char *args[])
{
    if (process_list == NULL) {
        print_log(LOG_WARN, "%s : process_list == NULL\n", __func__);
        return;
    }

    if (running_process < num_process) {
        // 起動数が足りない
        print_log(LOG_DEBUG, "%s : start new process : %u / %u\n", __func__, running_process, num_process);
        pid_t pid = fork();
        if (pid == 0) {
            // 子プロセス
            // stdin, stdoutを親プロセスとつなぐ
            // コマンドを実行する
            execvp(command, args);
        }
        else if (pid > 0) {
            // 親プロセス
            // 子プロセスのidを記録する
            spawner_process_t *p = find_free_slot();
            if (p == NULL) {
                // 空きスロット無し(bug)
                print_log(LOG_ERR, "%s : find_free_slot failed\n", __func__);
                // 子プロセスを終了する
                kill(pid, SIGTERM);
            }
            p->pid = pid;
            running_process++;
            print_log(LOG_INFO, "prosess start : %d\n", pid);
        }
        else {
            // error
            print_log(LOG_ERR, "%s : fork failed : %d\n", __func__, errno);
        }
    }
    else {
        // 何もしない
        usleep(10000);
    }
}

