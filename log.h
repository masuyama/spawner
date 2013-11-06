#if !defined(__LOG_H__INCLUDED__)
#define __LOG_H__INCLUDED__

#include    <stdarg.h>

/* log level */
#ifndef LOG_CRIT
#define LOG_CRIT    2       /* critical conditions */
#endif
#ifndef LOG_ERR
#define LOG_ERR     3       /* error conditions */
#endif
#ifndef LOG_WARNING
#define LOG_WARNING 4       /* warning conditions */
#endif
#ifndef LOG_WARN
#define LOG_WARN    LOG_WARNING /* alias */
#endif
#ifndef LOG_INFO
#define LOG_INFO    6       /* informational */
#endif
#ifndef LOG_DEBUG
#define LOG_DEBUG   7       /* debug */
#endif

#ifdef __cplusplus
extern "C"
{
#endif  /* ifdef __cplusplus */

int set_log_level(int new_level);
int print_log(int priority, const char* fmt, ...) __attribute__ ((format (printf, 2, 3)));
int print_proc(const char* fmt, ...) __attribute__ ((format (printf, 1, 2)));
//int print_journal(const char *prefix, const unsigned char *data, int datalen);

#ifdef __cplusplus
}
#endif  /* ifdef __cplusplus */

#endif /* !defined(__LOG_H__INCLUDED__) */
