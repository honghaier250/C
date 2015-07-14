#include <yaml.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

#define BUFFER_SIZE 65536

int print_output (char *name, unsigned char *buffer, size_t size, int count)
{
    FILE *file;
    char data[BUFFER_SIZE];
    size_t data_size = 1;
    size_t total_size = 0;
    if (count >= 0)
	{
	    printf ("FAILED (at the event #%d)\nSOURCE:\n", count + 1);
	}
    file = fopen (name, "rb");
    assert (file);
    while (data_size > 0)
	{
	    data_size = fread (data, 1, BUFFER_SIZE, file);
	    assert (!ferror (file));
	    if (!data_size)
		break;
	    assert (fwrite (data, 1, data_size, stdout) == data_size);
	    total_size += data_size;
	    if (feof (file))
		break;
	}
    fclose (file);
    printf ("#### (length: %d)\n", total_size);
    printf ("OUTPUT:\n%s#### (length: %d)\n", buffer, size);
    return 0;
}

int main (int argc, char *argv[])
{

    if (argc < 2)
	{
	    printf ("Usage: %s [-c] [-u] file1.yaml ...\n", argv[0]);
	    return 0;
	}

    unsigned char buffer[BUFFER_SIZE];
    size_t written = 1200;
    memset (buffer, 0, BUFFER_SIZE);

    print_output (argv[1], buffer, written, -1);

    return 0;
}
