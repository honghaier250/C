/* This is simple demonstration of how to use expat. This program
   reads an XML document from standard input and writes a line with the
   name of each element to standard output indenting child elements by
   one tab stop more than their parent element. */

#include <stdio.h>
#include <stdlib.h>
#include "xmlparse.h"

void startElement (void *userData, const char *name, const char **atts)
{
    int i;
    int *depthPtr = userData;
    for (i = 0; i < *depthPtr; i++)
        printf("\t");
    puts (name);

    for (i = 0; atts[i]; i += 2)
    {
        printf("\t%s='%s'", atts[i], atts[i + 1]);
    }

    printf("\n");
    *depthPtr += 1;
}

void endElement (void *userData, const char *name)
{
    int *depthPtr = userData;
    *depthPtr -= 1;
}

int main (int argc, char *argv[])
{
    char buf[BUFSIZ];
    XML_Parser parser = XML_ParserCreate (NULL);
    int done;
    int depth = 0;
    FILE *fp;

    if (argc != 2)
    {
        printf("Usage: %s filename\n", argv[0]);
        exit(0);

    }
    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        printf("Can't open %s\n", argv[1]);
        exit(1);

    }

    XML_SetUserData (parser, &depth);
    XML_SetElementHandler (parser, startElement, endElement);
    do
    {
        size_t len = fread (buf, 1, sizeof (buf), fp);
        // done = len < sizeof(buf);
        done = feof (fp);
        if (!XML_Parse (parser, buf, len, done))
        {
            fprintf (stderr, "%s at line %d\n", XML_ErrorString (XML_GetErrorCode (parser)), XML_GetCurrentLineNumber (parser));
            return 1;
        }
    }
    while (!done);
    XML_ParserFree (parser);
    return 0;
}
