#include <stdio.h>

int main(int argc, char *argv[], char *envp[])
{
puts("Pragma: no-cache");
puts("Cache-Control: no-cache");
puts("Content-Type: text/html\n\n");

while (*envp != NULL)
	{
	puts(*envp);
	puts("<BR>");
	envp++;
	}
}