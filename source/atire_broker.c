/*
	ATIRE_BROKER.C
	--------------
*/
#include <stdio.h>
#include "str.h"
#include "channel_file.h"
#include "channel_socket.h"
#include "atire_broker_param_block.h"
#include "atire_broke.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

const char * const PROMPT = ">";			// tribute to Woz

/*
	PROMPT()
	--------
*/
void prompt(ATIRE_broker_param_block *params)
{
if (params->port == 0)
	printf(PROMPT);
}

/*
	BETWEEN()
	---------
*/
char *between(char *source, char *open_tag, char *close_tag)
{
char *start,*finish;

if ((start = strstr(source, open_tag)) == NULL)
	return NULL;

start += strlen(open_tag);

if ((finish = strstr(start, close_tag)) == NULL)
	return NULL;

return strnnew(start, finish - start);
}

/*
	BROKER()
	--------
*/
void broker(ATIRE_engine *engine, ATIRE_broker_param_block *params)
{
ANT_channel *inchannel, *outchannel;
long long line;
long success;
char *pos;
char *command, *query;
char *new_index, *old_index, *new_doclist, *old_doclist;
long long first_to_list, page_size;

if (params->port == 0)
	{
	inchannel = new ANT_channel_file(params->queries_filename);		// stdin
	outchannel = new ANT_channel_file();							// stdout
	}
else
	inchannel = outchannel = new ANT_channel_socket(params->port);	// in/out to given port

line = 0;
prompt(params);

for (command = inchannel->gets(); command != NULL; prompt(params), command = inchannel->gets())
	{
	strip_space_inplace(command);
//	printf("->%s<-\n", command);
	if (*command != '\0')
		{
		if (strncmp(command, "<ATIREloadindex>", 16) == 0)
			{
			outchannel->puts("<ATIREloadindex>0</ATIREloadindex>");		// fail
			new_doclist = between(command, "<doclist>", "</doclist>");
			new_index = between(command, "<index>", "</index>");
			if (new_index == NULL || new_doclist == NULL)
				outchannel->puts("<ATIREloadindex>0</ATIREloadindex>");
			else
				{
				success = engine->load_index(new_index, new_doclist, &old_index, &old_doclist);
				delete [] old_index;
				delete [] old_doclist;
				if (success)
					outchannel->puts("<ATIREloadindex>1</ATIREloadindex>");
				else
					outchannel->puts("<ATIREloadindex>0</ATIREloadindex>");
				}
			}
		else if (strncmp(command, "<ATIREdescribeindex>", 18) == 0)
			{
			engine->describe_index(&old_index, &old_doclist);
			outchannel->puts("<ATIREdescribeindex>");

			outchannel->write("<doclist filename=\"");
			outchannel->write(old_doclist);
			outchannel->puts("\"/>");

			outchannel->write("<index filename=\"");
			outchannel->write(old_index);
			outchannel->puts("\"/>");
			outchannel->puts("</ATIREdescribeindex>");
			}
		else if (strncmp(command, "<ATIREsearch>", 13) == 0)
			{
			query = between(command, "<query>", "</query>");
			first_to_list = (pos = strstr(command, "<top>")) == NULL ? 1 : ANT_atoi64(pos + 5);
			page_size = (pos = strstr(command, "<n>")) == NULL ? params->results_list_length : ANT_atoi64(pos + 3);
			engine->search(query, first_to_list, page_size);
			delete [] query;
			}
		else if (strncmp(command, "<ATIREgetdoc>", 13) == 0)
			{
			}
		}
	delete [] command;
	}

delete inchannel;
if (outchannel != inchannel)
	delete outchannel;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ATIRE_broker_param_block params(argc, argv);
ATIRE_engine *engine;

params.parse();
engine = new ATIRE_broke(&params);
broker(engine, &params);

delete engine;
return 0;
}