#include "cc_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

static int cc_log_level = LOG_DEBUG;
static FILE *cc_log_file = NULL;

int set_log_level(int level)
{
    int old_level = cc_log_level;
    cc_log_level = level;
    return old_level;
}

int get_log_level(void)
{
    return cc_log_level;
}

int open_log_file(char *file)
{
    cc_log_file = fopen(file, "ab");
    if (NULL == cc_log_file)
    {
        fprintf(stderr, "%s:%d: %s\n", __FUNCTION__, __LINE__, "failed !");
        return -1;
    }

    return 0;
}

int close_log_file()
{
    fclose(cc_log_file);
    return 0;
}

void cc_log(int level, const char *function, int line, const char *format, ...)
{
    va_list ap;

    //以下为默认处理
    if (level > cc_log_level)
        return;
    if (NULL == cc_log_file)
    {
        cc_log_file = stderr;
    }

    switch (level)
    {
        case LOG_DEBUG:
            fprintf(cc_log_file, "[debug] %s:%d: ", function, line);
            break;
        case LOG_INFO:
            fprintf(cc_log_file, "[info] %s:%d: ", function, line);
            break;
        case LOG_NOTICE:
            fprintf(cc_log_file, "[notice] %s:%d: ", function, line);
            break;
        case LOG_WARNING:
            fprintf(cc_log_file, "[warning] %s:%d: ", function, line);
            break;
        case LOG_ERR:
            fprintf(cc_log_file, "[err] %s:%d: ", function, line);
            break;
        case LOG_CRIT:
            fprintf(cc_log_file, "[crit] %s:%d: ", function, line);
            break;
        case LOG_ALERT:
            fprintf(cc_log_file, "[alert] %s:%d: ", function, line);
            break;
        case LOG_EMERG:
            fprintf(cc_log_file, "[emerg] %s:%d: ", function, line);
            break;
        default:
            fprintf(cc_log_file, "[default] %s:%d: ", function, line);
            break;
    }

    va_start(ap, format);
    vfprintf(cc_log_file, format, ap);
    fprintf(cc_log_file, "\n");
    va_end(ap);
}
