#include <stdio.h>
#include <string.h>
#include "expat.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define BUFFSIZE  8192			//定义缓冲区的大小
char Buff[BUFFSIZE];			//创建一个缓冲区
int Depth;

//下面定义XML结构体
typedef struct page_struct
{

} page_struct;

//解码UTF-8
char *atx_string_strdup (const char *src)
{
	size_t size = 0;
	char *ret = 0;
	size = strlen (src);

	if (size)
	{
		ret = (char *) malloc (size + 2);
		if (ret)
		{
			strcpy (ret, src);
		}
	}

	return ret;
}

//下面定义一个XML元素开始处理的函数。
static void XMLCALL start (void *data, const char *el, const char **attr)
{
	int i;
	for (i = 0; i < Depth; i++)
		printf (" ");
	printf ("%s", el);

	for (i = 0; attr[i]; i += 2)
	{
		printf (" %s='%s'", atx_string_strdup (attr[i]), atx_string_strdup (attr[i + 1]));
	}

	printf ("\n");
	Depth++;
}								/* End of start handler */

//下面定义一个XML元素结束调用的函数。
static void XMLCALL end (void *data, const char *el)
{
	Depth--;
}

int main (int argc, char *argv[])
{
	//创建一个XML分析器
	XML_Parser p = XML_ParserCreate (NULL);

	//下面判断是否创建XML分析器失败。
	if (!p)
	{
		fprintf (stderr, "Couldn't allocate memory for parser\n");
		exit (-1);
	}
	//设置每个XML元素出现和结束的处理函数。这里设置start为元素开始处理函数，end元素结束处理函数。
	XML_SetElementHandler (p, start, end);
	//循环分析所有XML文件。
	FILE *file;
	char szDest[BUFFSIZE];
	memset (szDest, 0, BUFFSIZE);
	file = fopen ("tomcat-users.xml", "rb");
	for (;;)
	{
		int done;
		int len;
		//调用函数fread从文件里读取数据到缓冲区Buff里。
		len = (int) fread (Buff, 1, BUFFSIZE, file);
		if (ferror (file))
		{
			fprintf (stderr, "Read error\n");
			exit (-1);
		}
		//判断是否读取文件到结束
		done = feof (file);
		//EncodeToUTF8(Buff, szDest);
		//调用库函数XML_Parse来分析缓冲区Buff里的XML数据。
		if (XML_Parse (p, Buff, len, done) == XML_STATUS_ERROR)
		{
			fprintf (stderr, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n", XML_GetCurrentLineNumber (p), XML_ErrorString (XML_GetErrorCode (p)));
			exit (-1);
		}

		if (done)
			break;
	}
	XML_ParserFree (p);
	return 0;
}
