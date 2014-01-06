#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

static int log_level = 0;

int set_log_level(int new_level)
{
    int old_level = log_level;
    log_level = new_level;
    return old_level;
}

static int write_log_v(int priority, const char* fmt, va_list arg)
{
    char    buffer[8192];
    int     len;
    size_t  size;
    va_list aq;
    va_copy(aq, arg);

    size = sizeof(buffer);
    len = vsnprintf(buffer, size, fmt, aq);
    va_end(aq);

    if ((buffer[len - 1] != '\n') && ((len + 1) < (int)size)) {
        buffer[len] = '\n';
        buffer[len + 1] = '\0';
    }

    fprintf(stdout, "%1d> %s", priority, buffer);
    fflush(stdout);

    return 1;
}

int print_log(int priority, const char* fmt, ...)
{
    int ret;
    va_list arg;

    if (priority > log_level) {
        return 0;
    }

    va_start(arg, fmt);
    ret = write_log_v(priority, fmt, arg);
    va_end(arg);

    return ret;
}

int print_proc(const char* fmt, ...)
{
    int ret;
    va_list arg;

    va_start(arg, fmt);
    ret = write_log_v(0, fmt, arg);
    va_end(arg);

    return ret;
}

//int print_journal(const char *prefix, const unsigned char *data, int datalen)
//{
//    char encoded[base64encode_required_length(datalen)];
//    int enc_len = base64encode(data, datalen, encoded, sizeof(encoded));
//    if (enc_len <= 0) {
//        return -1;
//    }
//    encoded[enc_len] = '\0';
//    fprintf(stdout, "J> %s%s\n", prefix, encoded);
//    fflush(stdout);
//
//    return enc_len;
//}

