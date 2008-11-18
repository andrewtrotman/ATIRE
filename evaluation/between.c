#include <stdio.h>
#include <string.h>

char buffer[1024 * 1024];

int main(int argc, char *argv[])
{
FILE *fp;
char *ch;
long in;

if ((fp = fopen(argv[1], "r")) == NULL)
	return 0;

in = 0;
while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	if (strstr(buffer, "<DOCNO>") != NULL)
		in = 1;
	if (in)
		{
		for (ch = buffer; *ch != '\0'; ch++)
			if (*ch == '\n' || *ch == '\r')
				*ch = ' ';
		printf("%s", buffer);
		}
	if (strstr(buffer, "</DOCNO>") != NULL)
		{
		puts("");
		in = 0;
		}
	}	

fclose(fp);
}
