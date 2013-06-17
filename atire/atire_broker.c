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
#include "version.h"

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
long long line, documents, terms, length_of_longest_document, current_document_length;
long success;
char *pos, *command, *query, *hits;
char *new_index, *old_index, *new_doclist, *old_doclist;
long long first_to_list, page_size;
char *document_buffer;
long long length_of_document_buffer;
long broker_knows_total_number_of_documents;

/*
	Initialise I/O
*/
if (params->port == 0)
	{
	inchannel = new ANT_channel_file(params->queries_filename);		// stdin
	outchannel = new ANT_channel_file();							// stdout
	}
else
	inchannel = outchannel = new ANT_channel_socket(params->port);	// in/out to given port

/*
	Initilise the document buffer (used to read documents from the search engines)
*/
broker_knows_total_number_of_documents = false;
document_buffer = NULL;
length_of_document_buffer = length_of_longest_document = 0;

/*
	Initialise the instruction counter (nunmber of requests we're been asked to perform
*/
line = 0;

/*
	And now we can be a broker
*/
prompt(params);
for (command = inchannel->gets(); command != NULL; prompt(params), command = inchannel->gets())
	{
	strip_space_inplace(command);
	//printf("->%s<-\n", command);
	if (*command != '\0')
		{
		if (strncmp(command, "<ATIREloadindex>", 16) == 0)
			{
			*outchannel << "<ATIREloadindex>0</ATIREloadindex>" << ANT_channel::endl;		// fail
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
					*outchannel << "<ATIREloadindex>1</ATIREloadindex>" << ANT_channel::endl;
				else
					*outchannel << "<ATIREloadindex>0</ATIREloadindex>" << ANT_channel::endl;
				}
			}
		else if (strncmp(command, "<ATIREdescribeindex>", 18) == 0)
			{
			broker_knows_total_number_of_documents = true;
			engine->describe_index(&old_index, &old_doclist, &documents, &terms, &length_of_longest_document);

			*outchannel << "<ATIREdescribeindex>" << ANT_channel::endl;
			*outchannel << "<doclist filename=\"" << old_doclist << "\"/>" << ANT_channel::endl;
			*outchannel << "<index filename=\"" << old_index << "\"/>" << ANT_channel::endl;
			*outchannel << "<docnum>" << documents <<  "</docnum>" << ANT_channel::endl;
			*outchannel << "<termnum>" << terms << "</termnum>" << ANT_channel::endl;
			*outchannel << "<longestdoc>" << length_of_longest_document << "</longestdoc>" << ANT_channel::endl;
			*outchannel << "</ATIREdescribeindex>" << ANT_channel::endl;
			}
		else if (strncmp(command, "<ATIREsearch>", 13) == 0)
			{
			query = between(command, "<query>", "</query>");
			first_to_list = (pos = strstr(command, "<top>")) == NULL ? 1 : ANT_atoi64(pos + 5);
			page_size = (pos = strstr(command, "<n>")) == NULL ? params->results_list_length : ANT_atoi64(pos + 3);
			if ((hits = engine->search(query, first_to_list, page_size)) == NULL)
				*outchannel << "<ATIREerror>\nFailed to recieve any response from any server\n</ATIREerror>" << ANT_channel::endl;
			else
				*outchannel << hits << ANT_channel::endl;

			delete [] hits;
			delete [] query;
			}
		else if (strncmp(command, "<ATIREgetdoc>", 13) == 0)
			{
			/*
				Check to see that we know the length of the longest document in the collection
			*/
			if (!broker_knows_total_number_of_documents)
				engine->describe_index(&old_index, &old_doclist, &documents, &terms, &length_of_longest_document);

			/*
				Make sure we've allocates space enough to hold it
			*/
			if (length_of_longest_document >= length_of_document_buffer)
				{
				length_of_document_buffer = length_of_longest_document + 1;		// +1 for the '\0'
				delete [] document_buffer;
				document_buffer = new char [(size_t)length_of_document_buffer];
				}
			/*
				Now go get the document
			*/
			*document_buffer = '\0';
			current_document_length = 0;
			if ((current_document_length = length_of_longest_document) != 0)
				engine->get_document(document_buffer, &current_document_length, ANT_atoi64(strstr(command, "<docid>") + 7));

			/*
				And send it down the line to the client
			*/
			*outchannel << "<ATIREgetdoc>" << ANT_channel::endl;
			*outchannel << "<length>" << current_document_length << "</length>"  << ANT_channel::endl;
			outchannel->write(document_buffer, current_document_length);
			*outchannel << "</ATIREgetdoc>" << ANT_channel::endl;
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

if (params.logo)
	puts(ANT_version_string);

engine = new ATIRE_broke(&params);
broker(engine, &params);

delete engine;
return 0;
}
