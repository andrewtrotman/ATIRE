/*
	READABILITY_TAG_WEIGHTING.C
	---------------------------
 */

#include "readability_tag_weighting.h"
#include "unicode.h"
#include "directory_iterator_object.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
 * to add special term into vocab, it can't contain upper case letter, because it is for tag
 */
char *ANT_readability_TAG_WEIGHTING::special_tags[] = {"CATEGORY", "TITLE"};

/*
	ANT_READABILITY_TAG_WEIGHTING::~ANT_READABILITY_TAG_WEIGHTING()
	---------------------------------------------------------------
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
	---------------------------------------------------------------
*/
ANT_readability_TAG_WEIGHTING::~ANT_readability_TAG_WEIGHTING()
{
clean_up();
delete [] terms;
}

/*
	READABILITY_TAG_WEIGHTING::HANDLE_TAG()
	---------------------------------------
*/
void ANT_readability_TAG_WEIGHTING::clean_up()
{
if (term_count > 0)
	{
	for (int i = 0; i < term_count; ++i)
			delete terms[i];
	term_count = 0;
	}
tag_processing_on = FALSE;
where = -1;
matching_tag = NULL;
}


/*
	READABILITY_TAG_WEIGHTING::HANDLE_TAG()
	---------------------------------------
*/
void ANT_readability_TAG_WEIGHTING::handle_tag(ANT_parser_token *tag, long tag_open, ANT_parser *parser)
{
if (tag_open)
	{
	for (int i = 0; i < number_of_tags; ++i)
		if (strncmp(tag->start, special_tags[i], tag->string_length) == 0)
			{
			matching_tag = special_tags[i];
			where = i;
			prefix_char = ANT_tolower(matching_tag[0]);
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

void ANT_readability_TAG_WEIGHTING::handle_token(ANT_parser_token *token)
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
	 	Wikipedia abstract file dump give title in such a way "wikipedia : XXXXX", term count is 1 when encounter the colon;
		for for Chinese, the unicode colon ':' = "\357\274\232" in OCT, and the term count for word Wikipedia is 4.
	 */
	if ((term_count == 1 && strncmp(token->start, ":", token->string_length) == 0)
			|| (term_count == 4 && strncmp(token->start, "\357\274\232", token->string_length) == 0)
			/*&& strcmp(wiki_prefix, terms[0]) == 0*/)
		{
			while (term_count > 0)
				delete terms[--term_count];

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
void ANT_readability_TAG_WEIGHTING::index(ANT_memory_indexer *indexer, long long doc, ANT_directory_iterator_object *current_file)
{
if (term_count == 0)
	return;

/*
  the boundary of the buffer wasn't checked assuming the text in the tile or category node won't be longer than that
 */

// now we add the full title, full category information in the dictionary
char buffer[MAX_TERM_LENGTH];
char *start, *info_buf_start;
ANT_parser_token what;
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
		indexer->add_term(&what, doc, 20);
		start =  buffer + 2;
		}

/*
   Now the full text enclosed in the node, for example, title "Bill Clinton"
   we will put "TF:bill clinton" into dictionary
 */

info_buf[1] = 'f';
info_buf[2] = ':';

info_buf_start = info_buf + 3;
*info_buf_start = '\0';

what.start = info_buf;
what.string_length = length + 3;

std::string title(current_file->filename);
unscape_xml(title);

length = title.length();

memcpy(info_buf_start, title.c_str(), length);
//info_buf_start += length;
*(info_buf_start + length) = '\0';
what.string_length += length;

while (*info_buf_start != '\0')
 info_buf_start = utf8_tolower(info_buf_start);

/* the following solution doesn't work well. we have to have the exact title as it is */
/*

length = strlen(terms[0]);
memcpy(info_buf_start, terms[0], length);
info_buf_start += length;
*info_buf_start = '\0';

int is_first_term_punct = ANT_ispunct(terms[0][0]) || utf8_ispuntuation(terms[0]);

for (i = 1; i < term_count; ++i)
	{
	if (!is_first_term_punct && !is_cjk_language(terms[i]) && !ANT_ispunct(terms[i][0]) && !utf8_ispuntuation(terms[i])) // we need to restore the title, so only put spaces between characters that are not puntuations
		{
		*info_buf_start++ = ' ';
		what.string_length++;
		}
    is_first_term_punct = ANT_ispunct(terms[i][0]) || utf8_ispuntuation(terms[i]);
	length = strlen(terms[i]);
	memcpy(info_buf_start, terms[i], length);
	info_buf_start += length;
	*info_buf_start = '\0';
	what.string_length += length;
	}
*/

indexer->add_term(&what, doc, 20); // avoid being culled of optimization

clean_up();
}

/*
	ANT_READABILITY_DALE_CHALL::UNSCAPE_XML()
	-----------------------------------
*/
void ANT_readability_TAG_WEIGHTING::unscape_xml(std::string& source)
{
	static const char *entities[] = {"&quot;", "&lt;", "&gt;", "&apos;", "&amp;" };
	static const char *to_chars[] = {"\"", "<", ">", "'", "&" };
	static const std::size_t num = sizeof(entities)/sizeof(entities[0]);

	std::size_t j = 0;
	for (std::size_t i = 0; i < num; ++i)
		for (;(j = source.find(entities[i], j)) != std::string::npos;)
			{
			source.replace(j, strlen(entities[i]), to_chars[i]);
			}
}

