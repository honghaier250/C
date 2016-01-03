#ifndef __KL_CC_LOG_H__
#    define __KL_CC_LOG_H__

#    include <syslog.h>
#    include <stdarg.h>

#    define CC_LOG(level, f, ...)  cc_log((level), __FUNCTION__, __LINE__, (f), ##__VA_ARGS__)

#    define cc_debug(f, ...)  CC_LOG(LOG_DEBUG, (f), ##__VA_ARGS__)
typedef void (*CC_LOG_HANDLE) (int level, const char *function, int line, const char *format, va_list ap);
extern void cc_log(int level, const char *function, int line, const char *format, ...);
int set_log_level(int level);
int get_log_level(void);

#endif                            //__KL_CC_LOG_H__
