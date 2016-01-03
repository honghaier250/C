#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mysql.h"
#include "cc_log.h"

#define CFGFILE "CentralControl.conf"
static char *server;
static char *port;
static char *user;
static char *passwd;
static char *log;

static char *strtok_quote(char *line, char *sep, char **quote_ptr)
{
    int inquote;
    char *tmp;
    static char *next;

    *quote_ptr = NULL;
    if (line != NULL)
    {
        next = line;
    }
    while (*next && strchr(sep, *next))
    {
        next++;
    }

    if (*next == '\0')
    {
        next = NULL;
        return (NULL);
    }
    tmp = next;

    for (inquote = 0; *next;)
    {
        switch (*next)
        {
            case '"':
                if (inquote)
                {
                    inquote = 0;
                }
                else
                {
                    inquote = 1;
                }
                memcpy(next, next + 1, strlen(next + 1) + 1);
                break;

            case '\\':
                if (next[1])
                    memcpy(next, next + 1, strlen(next + 1) + 1);
                next++;
                break;

            default:
                if (!inquote)
                {
                    if (strchr(sep, *next) != NULL)
                    {
                        *quote_ptr = next;
                        *next++ = '\0';
                        return (tmp);
                    }
                }
                next++;
                break;
        }
    }

    return (tmp);
}

/**
 * 按行解析配置文件
 */
int config_fp_parse_line(char *p)
{
    int i = 0;
    char *token;
    char *value;
    char *quote_ptr;
    static char *const hide[] = { "server", "port", "user", "passwd", "log", NULL };
    int count = (int) (sizeof(hide) / sizeof(hide[0])) - 1;

    //首字母为'#'视为注释行
    if ('#' == *p)
    {
        return 0;
    }

    token = strtok_quote(p, " \t=", &quote_ptr);

    //检测配置指令是否正确
    if (token)
    {
        for (i = 0; hide[i]; i++)
        {
            if (!strcasecmp(token, hide[i]))
                break;
        }
    }

    if (count <= i)
    {
        CC_LOG(LOG_ERR, "%s", "config directive error !");
        return -1;
    }

    /* if (quote_ptr) */
    /*     *quote_ptr = ' '; */
    /* if (quote_ptr) */
    /*     *quote_ptr = '\0'; */

    for (; value = strtok_quote(NULL, " \t=", &quote_ptr);)
    {
        if (value == NULL)
            break;

        if (!strcasecmp(token, "server"))
        {
            server = strdup(value);
        }
        else if (!strcasecmp(token, "port"))
        {
            port = strdup(value);
        }
        else if (!strcasecmp(token, "user"))
        {
            user = strdup(value);
        }
        else if (!strcasecmp(token, "passwd"))
        {
            passwd = strdup(value);
        }
        else
        {
            log = strdup(value);
        }

    }

    return 0;
}

/**
 * 解析配置文件
 */
int config_fp_parse(char *file)
{
    char *p = NULL;
    char *token = NULL;
    char *quote_ptr;
    FILE *fp = NULL;
    char buf[512];

    fp = fopen(file, "rb");
    if (NULL == fp)
    {
        CC_LOG(LOG_ERR, "%s %s", file, "open failed!");
        return -1;
    }

    while (fgets(buf, sizeof(buf), fp))
    {
        p = strchr(buf, '\n');
        if (p)
        {
            if (p > buf && p[-1] == '\r')
            {
                --p;
            }
            *p = '\0';
        }

        //跳过空格
        p = buf;
        while (isspace(*p))
        {
            p++;
        }

        config_fp_parse_line(p);
    }

    fclose(fp);
    return 0;
}

int cc_init(char *sql, char *token, char *psUserInfo)
{
    int ret = 0;
    token = "48798gdf56sd23tg1";

    ret = config_fp_parse(CFGFILE);
    if (0 > ret)
    {
        CC_LOG(LOG_ERR, "%s", "cfg file parse failed!");
        return -1;
    }

    if (NULL != log)
    {
        ret = open_log_file(log);
        if (0 != ret)
        {
            CC_LOG(LOG_ERR, "%s", "open log file failed!");
        }
    }

    if (NULL != psUserInfo)
    {
        memset(psUserInfo, 0, sizeof(psUserInfo));
    }

    if (NULL == port)
    {
        port = strdup("3306");
    }
    if (NULL == user)
    {
        user = strdup("root");
    }
    if (NULL == passwd)
    {
        passwd = strdup("admin");
    }

    sprintf(sql, "SELECT OBJECTID,NAME,ID_CARD,PAPERS FROM TB_PMS_SUBJECT_RES WHERE CERT_SN= '%s' ", token);
    return 0;
}

void cc_finish(MYSQL mysql, MYSQL_RES * result)
{
    if (NULL != server)
    {
        free(server);
    }
    if (NULL != port)
    {
        free(port);
    }
    if (NULL != user)
    {
        free(user);
    }
    if (NULL != passwd)
    {
        free(passwd);
    }
    if (NULL != log)
    {
        close_log_file();
        free(log);
    }
    if (NULL != result)
    {
        mysql_free_result(result);
    }
    if (NULL != &mysql)
    {
        mysql_close(&mysql);
    }
}

int process_result(MYSQL_RES * result, char *psUserInfo)
{
    int i = 0;
    int ret = 0;
    int cols = 0;
    char *name = NULL;
    char *field = NULL;
    MYSQL_ROW rows;

    cols = mysql_num_fields(result);
    rows = mysql_fetch_row(result);
    for (i = 0; i < cols; i++)
    {
        field = mysql_fetch_fields(result)[i].name;
        if (0 == strcasecmp(field, "OBJECTID"))
        {
            name = strdup("id");
        }
        else if (0 == strcasecmp(field, "NAME"))
        {
            name = strdup("xm");
        }
        else if (0 == strcasecmp(field, "ID_CARD"))
        {
            name = strdup("zjhm");
        }
        else if (0 == strcasecmp(field, "PAPERS"))
        {
            name = strdup("sfhm");
        }

        if (i == 0)
        {
            sprintf(psUserInfo, "%s", "/");
        }
        if ((i + 1) == cols)
        {
            sprintf(psUserInfo, "%s%s%s%s", psUserInfo, name, "=", *(rows + i));
            free(name);
            break;
        }

        sprintf(psUserInfo, "%s%s%s%s%s", psUserInfo, name, "=", *(rows + i), "/");
        free(name);
    }

    return 0;
}

int JzgkVerifyToken(char *key, char *token, char *psUserInfo)
{
    int ret = 0;
    MYSQL mysql;
    MYSQL_RES *result;
    char sql[120] = { 0 };

    ret = cc_init(sql, token, psUserInfo);
    if (0 != ret)
    {
        CC_LOG(LOG_ERR, "%s", "cc_init failed !");
    }

    mysql_init(&mysql);
    if (!mysql_real_connect(&mysql, server, user, passwd, "xdgw", atoi(port), NULL, 0))
    {
        CC_LOG(LOG_ERR, "%s", "mysql connect failed !");
        return -1;
    }

    ret = mysql_real_query(&mysql, sql, (unsigned long) strlen(sql));
    if (0 != ret)
    {
        CC_LOG(LOG_ERR, "%s", "select failed !");
        return -1;
    }

    result = mysql_store_result(&mysql);
    if (NULL == result)
    {
        CC_LOG(LOG_ERR, "%s", "stroe result failed !");
        return -1;
    }

    ret = process_result(result, psUserInfo);
    if (0 != ret)
    {
        CC_LOG(LOG_ERR, "%s", "process_result failed !");
    }

    cc_finish(mysql, result);
    return 0;
}
