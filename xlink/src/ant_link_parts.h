/*
	LINK_PARTS.H
	------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/

#ifndef __LINK_PARTS_H__
#define __LINK_PARTS_H__

#include <ctype.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "language.h"

#define INEX_ARCHIVE_ARTICLE_ID_SIGNITURE "name id="
#define ARTICLE_ID_SIGNITURE "id"
#define ARTICLE_NAME_SIGNITURE "title"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

/*
	STRING_CLEAN()
	--------------
*/
inline static char *string_clean(char *file, long lower_case_only = 1, long trim = 1)
{
char *ch, *from, *to;

/*
	remove XML tags and remove all non-alnum (but keep case)
*/
ch = file;
while (*ch != '\0')
	{
	if (*ch == '<')			// then remove the XML tags
		{
		while (*ch != '>')
			*ch++ = ' ';
		*ch++ = ' ';
		}
	else if (!isalnum(*ch))	// then remove it
		*ch++ = ' ';
	else
		{
		if (lower_case_only)
			{
			*ch = (char)tolower(*ch);
			ch++;
			}
		else
			ch++;
		}
	}

/*
	now remove multiple, head, and tail spaces.
*/
if (trim) {
	from = to = file;
	while (isspace(*from))
		from++;
	while (*from != '\0')
		{
		while (isalnum(*from))
			*to++ = *from++;
		if (isspace(*from))
			*to++ = *from++;
		while (isspace(*from))
			from++;
		}
	if (to > file && isspace(*(to - 1)))
		to--;
	*to = '\0';
}

return file;
}

/*
	STRING_CLEAN()
	--------------
*/
inline static char *string_clean_tag(char *file, long trim = 1)
{
char *ch, *from, *to;

/*
	remove XML tags and remove all non-alnum (but keep case)
*/
ch = file;
while (*ch != '\0')
	{
	if (*ch == '<')			// then remove the XML tags
		{
		while (*ch != '>')
			*ch++ = ' ';
		*ch++ = ' ';
		}
	else
		ch++;
	}

/*
	now remove multiple, head, and tail spaces.
*/
if (trim) {
	from = to = file;
	while (isspace(*from))
		from++;
	while (*from != '\0')
		{
		while (isalnum(*from))
			*to++ = *from++;
		if (isspace(*from))
			*to++ = *from++;
		while (isspace(*from))
			from++;
		}
	if (to > file && isspace(*(to - 1)))
		to--;
	*to = '\0';
}

return file;
}

/*
	STRNNEW()
	---------
*/
inline static char *strnnew(char *source, long long length)
{
char *ans;

ans = new char [(size_t)(length + 1)];
strncpy(ans, source, (size_t)length);
ans[length] = '\0';

return ans;
}

/*
	GET_DOC_ID()
	------------
*/
inline static long get_doc_id(const char *file)
{
char *pos;

pos = strstr((char*)file, "<"INEX_ARCHIVE_ARTICLE_ID_SIGNITURE);
if (pos == NULL)
	pos = strstr((char*)file, "<"ARTICLE_ID_SIGNITURE">");
if (pos == NULL) {
	fprintf(stderr, "Cannot find DOC id <name id=...> or <id> in file\n");
	return -1;
}
while (!isdigit(*pos))
	pos++;

return atol(pos);
}

inline static long result_to_id(const char *source)
{
	const char *target_start = source;/*, *target_end, *target_dot*/
	char *slash = 0;

	while (target_start && (slash = strpbrk((char*)target_start, "\\/")))
		target_start = slash + 1;

	if (!target_start)
		return -1;

	const char *pos = target_start;
	while (!isdigit(*pos))
		pos++;

	return atol(pos);
}

/*
	 GET_DOC_NAME()
	 --------------
 */
inline char *get_doc_name(char *file, char *to)
{
	char *pos, *start, *end;
	long len = 0;

	pos = strstr(file, "<"ARTICLE_NAME_SIGNITURE);
	if (pos == NULL) {
		fprintf(stderr, "Cannot find DOC title in file\n");
		return to;
	}

	start = strchr(pos, '>');
	start++;
	end = strstr(file, "</"ARTICLE_NAME_SIGNITURE);
	len = end - start;
	strncpy(to, start, len);
	to[len] = '\0';

	return to;
}

/*
	 GET_TEARA_DOC_NAME()
	 --------------
 */
inline char *get_teara_doc_name(char *file, char *to)
{
	char *pos, *start, *end, *last = file;
	long len = 0;

	if (*file != '\0') {
		pos = strstr(file, "<Name"); // TeAra corpus name tag
		if (pos == NULL) {
			fprintf(stderr, "Cannot find DOC title in file\n");
			last = file + strlen(file);
		}
		else {
			start = strchr(pos, '>');
			start++;
			end = strstr(file, "</Name");
			len = end - start;
			strncpy(to, start, len);
			to[len] = '\0';
			last = end + 6;
		}
	}
	return last;
}

/*
	 GET_TEARA_DOC_NAME2()
	 --------------
 */
inline char *get_teara_doc_name2(char *file, char *to)
{
	char *pos, *start, *end, *last = file;
	long len = 0;

	if (*file != '\0') {
		pos = strstr(file, "Name"); // TeAra corpus name tag
		if (pos == NULL) {
			//fprintf(stderr, "Cannot find DOC title in file\n");
			last = file + strlen(file);
		}
		else {
			start = strchr(pos, '>');
			start++;
			end = strstr(start, "</");
			len = end - start;
			strncpy(to, start, len);
			to[len] = '\0';
			char *temp = strstr(end, "Name");;
			if (temp != NULL)
				last = temp + 4;
			else
				last = end + 2;
		}
	}
	return last;
}

/*
	 STRING_TOLOWER()
	 ----------6
 */
inline char *string_tolower(char *source, bool replac_hyphen = false)
{
int i = 0;
for (; i < strlen(source); i++) {
	if (source[i] >= 'A' && source[i] <= 'Z')
		source[i] = tolower(source[i]); // - 'A' + 'a';
	else if (source[i] == '-' && replac_hyphen)
		source[i] = ' ';
}
return source;
}

/*
	COUNT_CHAR()
	------------
*/
inline long count_char(char *buffer, char what)
{
char *ch;
long times;

times = 0;
for (ch = buffer; *ch != '\0'; ch++)
	if (*ch == what)
		times++;

return times;
}

/*
 * 	STRING_TO_LIST()
 * 	----------------
 */
inline int string_to_list(char *source, char **list)
{
	//const char *seperators = " \t\n\r.()\\/\"";
	char **current;
	char *token;
	int count = 0;

	current = list;// = new char *[strlen(source)];		// this is the worst case by far
//	for (token = strtok(source, seperators); token != NULL; token = strtok(NULL, seperators))
//		*current++ = token;
	while(*source != '\0') {
		while(*source != '\0' && !isalnum(*source))
			++source;

		if (*source != '\0') {
			*current++ = source;
			while(*source != '\0' && isalnum(*source))
				++source;
			if (*source != '\0') {
				*source = '\0';
				++source;
			}
		}
	}
	count = current - list;
	*current = NULL;
	return count;
}

/*
 * 	STRING_TO_LIST()
 * 	----------------
 */
inline char **string_to_list(char *source)
{
	char **term_list = new char *[strlen(source)];
	string_to_list(source, term_list);
	return term_list;
}

/*
 * FIND_PHRASE()
 * -------------
 * @return offset of the phrase position
 */
inline long find_phrase(const char *source, const char *phrase, long *offset/*char *where*/)
{
	char buffer[1024 * 1024];
	char *where_to, *first_found = NULL, *curr_found = NULL;;
	char **term_list, **first, **last, **current;
	char **phrase_list, **term;
//	long *offset;
//	long pos = 0;

	char *copy =  strdup(source);
	char *phrasecopy = strdup(phrase);

	term_list = new char *[strlen(source)];
	phrase_list = new char *[strlen(phrase)];

	int source_terms_count = string_to_list(copy, term_list);
	int phrase_terms_count = string_to_list(phrasecopy, phrase_list);

//	for (term = phrase_list; term != NULL; term++)
//		terms_count++;

//	offset = new long[phrase_terms_count];
//
//	for (int i = 0; i < phrase_terms_count ; i++)
//		offset[i] = -1;
	//long offset = 0;
	long first_length = 0, curr_length = 0;

	//bool stop_flag = false;
	//bool repeat_flag = false;
	int term_index = 0;
	int index = term_index;

	do {
		//repeat_flag = true;
		index = term_index;
		for (first = term_list; *first != NULL && index < phrase_terms_count; first++) {
			for (last = first; *last != NULL && index < phrase_terms_count; last++) {
				char *temp_term = phrase_list[index];
				if (/*index < phrase_terms_count && */strcmp(*last, temp_term) == 0) {
					//offset[index] = (*last) - copy;
					if (index == term_index) {
						curr_found = *last;
						curr_length = strlen(*last);
					} else
						curr_length = *last - curr_found + strlen(*last);

					//repeat_flag = false;
					index++;
					continue;
				}

//				if ((index + 1) == phrase_terms_count) {
//					index = 0;
//					stop_flag = true;
//				}

				index = term_index;
				break;
			}
//			if (stop_flag)
//				break;

			if (curr_length > first_length) {
				first_length = curr_length;
				first_found = curr_found;
			}
		}
//		if (stop_flag)
//			break;
		++term_index;
	}
	while (/*repeat_flag && index < phrase_terms_count && */!first_found && term_index < phrase_terms_count);

	//pos = offset[0];

	delete [] term_list;
	delete [] phrase_list;
	free(copy);
	free(phrasecopy);
	if (first_found)
		*offset = first_found - copy;

	//where = (char *)source + offset;
	return first_length;
}

/*
 	STRING_REMOVE_SPACE()
	--------------------
*/
inline void string_remove_space(char *s)
{
char *last, *previous, *temp;
long len, has_space;
has_space = FALSE;
last = s + strlen(s);
while (last != s)
	{
	previous = last - 1;
	if (*previous == ' ')
		{
		has_space = TRUE;
		while (*previous == ' ' && previous >= s)
			--previous;
		++previous;
		}
	if (has_space)
		{
		len = strlen(last);
		memmove(previous, last, len);
		previous[len] = '\0';
		has_space = FALSE;
		}

	last = previous;
	}
}

/*
	UTF8_TOKEN_COMPARE()
	--------------------
*/
inline int utf8_token_compare(const char *s1, const char *s2, long *is_substring = NULL)
{
int cmp, i, min_len;
char *new_s1 = strdup(s1);
char *new_s2 = strdup(s2);

if (strchr(new_s1, ' ') != NULL)
	string_remove_space(new_s1);

if (strchr(new_s2, ' ') != NULL)
	string_remove_space(new_s2);

min_len = MIN(strlen(new_s1), strlen(new_s2));

cmp = memcmp(new_s1, new_s2, min_len);

if (cmp == 0)
	{
	if (strlen(new_s1) <= strlen(new_s2))
		{
		if (is_substring != NULL)
			*is_substring = TRUE;
		if (strlen(new_s1) < strlen(new_s2))
			cmp = -1;
		}
	else
		cmp = 1;
	}

free(new_s1);
free(new_s2);
return cmp;
}

/*
	CREATE_UTF8_TOKEN_LIST()
	------------------------
*/
inline int create_utf8_token_list(char *s, char **term_list)
{
char *start, *token, *where_to = s;
long token_len = 0, term_count;
char **current = term_list;

term_count = 0;
while (*where_to != '\0')
	{
	while (isspace(*where_to))
		++where_to;

	start = where_to;
	if ((*where_to & 0x80) &&language::isutf8(where_to))
		{
		token_len = language::utf8_bytes(where_to);
		where_to += token_len;
		}
	else
		while (*where_to != '\0' && !isspace(*where_to) &&  !((*where_to & 0x80) && language::isutf8(where_to)))
			{
			++token_len;
			++where_to;
			}

	*current = token = new char[token_len + 1];
	strncpy(*current, start, token_len);
	token[token_len] = '\0';
	++current;
	token_len = 0;
	++term_count;
	}

*current = NULL;
return term_count;
}

/*
	FREE_UTF8_TOKEN_LIST()
	----------------------
*/
inline void free_utf8_token_list(char **term_list)
{
char **current = term_list;
while (*current != NULL)
	{
	delete [] *current;
	++current;
	}
}

/*
	STRING_COMPARE()
	----------------
*/
inline int string_compare(const char *s1, const char *s2, long compare_without_space = 0)
{
int min_len, cmp;
if (compare_without_space && (strchr(s1, ' ') != NULL || strchr(s2, ' ') != NULL)) // we don't need token comparison for all, only those has space in it
	cmp = utf8_token_compare(s1, s2);
else
	{
	min_len = strlen(s1) > strlen(s2) ? strlen(s2) : strlen(s1);
	cmp = memcmp(s1, s2, min_len);
	if (cmp == 0 && strlen(s1) != strlen(s2))
		cmp = strlen(s1) < strlen(s2) ? -1 : 1;
	}
return cmp;
}

#endif /* __LINK_PARTS_H__ */

