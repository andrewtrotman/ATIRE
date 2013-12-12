/*
    TREC2QUERY.C
    ------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "../source/channel_file.h"
#include "../source/channel_trec.h"

using namespace std;

/*
    MAIN()
    ------
*/
int main(int argc, char *argv[])
{
ANT_channel *inchannel;
ANT_channel *outchannel;
char *inchannel_word;

if (argc != 3)
    exit(printf("Usage:%s <trectopicfile> <tag>\n<tag> is any combination of T, D, N (title, desc, narr)", argv[0]));

inchannel = new ANT_channel_file(argv[1]);
outchannel = new ANT_channel_file();

inchannel = new ANT_channel_trec(inchannel, argv[2]);
for (inchannel_word = inchannel->gets(); inchannel_word != NULL; inchannel_word = inchannel->gets())
    {
    outchannel->puts(inchannel_word);
    }

if (outchannel != inchannel)
    delete outchannel;

delete inchannel;

return EXIT_SUCCESS;
}
