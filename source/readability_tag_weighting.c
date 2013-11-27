/*
 * readability_TAG_WEIGHTING.cpp
 *
 *  Created on: 23/11/2013
 *      Author: monfee
 */

#include "readability_tag_weighting.h"
#include "unicode.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

char **ANT_readability_TAG_WEIGHTING::special_tags = (char *[]) {"CATEGORY", "TITLE"};

/*
	ANT_READABILITY_TAG_WEIGHTING::~ANT_READABILITY_TAG_WEIGHTING()
	---------------------------------------------------
*/
ANT_readability_TAG_WEIGHTING::ANT_readability_TAG_WEIGHTING()
{
	number_of_tags = 2;
	matching_tag = NULL;
	where = -1;
	term_count = 0;
	tag_processing_on = FALSE;
	terms = new char*[MAX_TERM_COUNT + 1];
}

/*
	ANT_READABILITY_TAG_WEIGHTING::~ANT_READABILITY_TAG_WEIGHTING()
	---------------------------------------------------
*/
ANT_readability_TAG_WEIGHTING::~ANT_readability_TAG_WEIGHTING()
{
clean_up();
delete [] terms;
}

/*
	READABILITY_TAG_WEIGHTING::HANDLE_TAG()
	----------------------------------
*/
void ANT_readability_TAG_WEIGHTING::clean_up()
{
for (int i = 0; i < term_count; ++i)
		delete terms[i];
tag_processing_on = FALSE;
where = -1;
matching_tag = NULL;
term_count = 0;
}


/*
	READABILITY_TAG_WEIGHTING::HANDLE_TAG()
	----------------------------------
*/
void ANT_readability_TAG_WEIGHTING::handle_tag(ANT_string_pair *tag, long tag_open, ANT_parser *parser)
{
if (tag_open)
	{
	for (int i = 0; i < number_of_tags; ++i)
		if (strncmp(tag->start, special_tags[i], tag->string_length) == 0)
			{
			matching_tag = special_tags[i];
			where = i;
			prefix_char = ANT_toupper(matching_tag[0]);
			tag_processing_on = TRUE;
			should_segment = parser->get_segment_info();
			parser->set_segment_info(0);
			break;
			}
	}
else
	{
	//if (strncmp(tag->start, matching_tag, tag->string_length) == 0) // we are not checking if the closing tag is the one being opened at the moment
	tag_processing_on = FALSE;
	parser->set_segment_info(should_segment);
	}
}

void ANT_readability_TAG_WEIGHTING::handle_token(ANT_string_pair *token)
{
/*
  the title may begin with "Wikipedia:", if term_count is greater than 1, then we ignoring it
  it is not best solution, but we can live with it.
 */
static const char *wiki_prefix = "wikipedia";
char *term, *start;

if (tag_processing_on && term_count <= MAX_TERM_COUNT)
	{
	/*
	 	Wikipedia abstract file dump give title in such a way "wikipedia : XXXXX"
		unicode colon ':' = "\357\274\232" in OCT
	 */
	if (term_count == 1 && (strncmp(token->start, "\357\274\232", token->string_length) == 0 || strncmp(token->start, ":", token->string_length) == 0)
			&& strcmp(wiki_prefix, terms[0]) == 0)
		{
			delete terms[0];
			term_count = 0;
			return;
		}

	terms[term_count] = strnnew(token->start, token->string_length);
	term = start = terms[term_count];
	while (start != NULL && (start - term) < strlen(term))
		start = utf8_tolower(start);
	++term_count;
	}
}

/*
	ANT_READABILITY_DALE_CHALL::INDEX()
	-----------------------------------
*/
void ANT_readability_TAG_WEIGHTING::index(ANT_memory_indexer *indexer, long long doc)
{
if (term_count == 0)
	return;

/*
  the boundary of the buffer wasn't checked assuming the text in the tile or category node won't be longer than that
 */

// now we add the full title, full category information in the dictionary
char buffer[MAX_TERM_LENGTH];
char *start, *info_buf_start;
ANT_string_pair what;
long long length = 0;
int i;

what.start = buffer;
buffer[0] = info_buf[0] = prefix_char;
start = buffer + 1;
*start++ = ':';

/*
	put each special term into index, but term count has be greater than 1
 */
if (term_count > 1)
	for (i = 0; i < term_count; ++i)
		{
	//	if (prefix_char == 'T' && i == 0)
	//		continue;

		length = strlen(terms[i]);
		memcpy(start, terms[i], length);
		start += length;
		*start = '\0';
		what.string_length = length + 2; // including prefix string "C:" or "T:"
		indexer->add_term(&what, doc, 10);
		start =  buffer + 2;
		}

/*
   Now the full text enclosed in the node, for example, title "Bill Clinton"
   we will put "TF:bill clinton" into dictionary
 */

info_buf[1] = 'F';
info_buf[2] = ':';

info_buf_start = info_buf + 3;
*info_buf_start = '\0';

length = strlen(terms[0]);
memcpy(info_buf_start, terms[0], length);
info_buf_start += length;
*info_buf_start = '\0';
what.start = info_buf;
what.string_length = length + 3;

for (i = 1; i < term_count; ++i)
	{
	if (!ischinese(terms[i]))
		{
		*info_buf_start++ = ' ';
		what.string_length++;
		}
	length = strlen(terms[i]);
	memcpy(info_buf_start, terms[i], length);
	info_buf_start += length;
	*info_buf_start = '\0';
	what.string_length += length;
	}

indexer->add_term(&what, doc, 10); // avoid being culled of optimization

clean_up();
}
