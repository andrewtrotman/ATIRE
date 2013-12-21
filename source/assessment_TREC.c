/*
	ASSESSMENT_TREC.C
	-----------------
	This code reads assessment files in the TREC native format
	That format is:
	TopicID (0|SubtopicID) DocID Rel
*/
#include <string.h>
#include "hash_table.h"
#include "assessment_TREC.h"
#include "relevant_document.h"
#include "memory.h"
#include "disk.h"
#include "str.h"
#include "instream_file.h"
#include "instream_deflate.h"
#include "instream_buffer.h"
#include "instream_memory.h"
#include "directory_iterator_tar.h"
#include "directory_iterator_pkzip.h"
#include "compress_text_deflate.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_ASSESSMENT_TREC::READ_ENTIRE_FILE()
	---------------------------------------
	read the assessment file.  If its a .tar.gz then read each piece and join them together.
*/
char *ANT_assessment_TREC::read_entire_file(char *filename)
{
ANT_instream *file_stream, *decompressor, *instream_buffer;
ANT_directory_iterator *source;
long which;
static const long MAX_PARTS = 10;
char *into, *total, *part[MAX_PARTS];
long long length[MAX_PARTS], total_size, current;
ANT_memory file_buffer(10 * 1024 * 1024);
ANT_directory_iterator_object file_object, *piece;

if (strrcmp(filename, ".zip") == 0)
	{
	ANT_directory_iterator_pkzip source(filename);

	if (source.first(&file_object) == NULL)
		exit(printf("Cannot load assessment file:%s\n", filename));
	return file_object.file;
	}
else if ((strrcmp(filename, ".tar.gz") == 0) || (strrcmp(filename, ".tgz") == 0))
	{
	file_stream = new ANT_instream_file(&file_buffer, filename);
	decompressor = new ANT_instream_deflate(&file_buffer, file_stream);
	instream_buffer = new ANT_instream_buffer(&file_buffer, decompressor);
	source = new ANT_directory_iterator_tar(instream_buffer, ANT_directory_iterator::READ_FILE);

	which = 0;
	total_size = 0;
	for (piece = source->first(&file_object); piece != NULL; piece = source->next(&file_object))
		{
		if (strrcmp(piece->filename, ".gz") == 0)
			{
			/*
				The .tar.gz is full of .gz files (like, whose idea was that?).  One of the sucky things about
				.gz files is that you can't know how big the decompressed file is going to be because it isn't
				stored anywhere.  We're going to allow at most 10 times the raw size and fail it its bigger
				because someting has probably gone wrong in that case.
			*/
			ANT_memory inflate_memory(1024 * 1024);
			ANT_instream_memory source_buffer(piece->file, piece->length);
			ANT_instream_deflate inflate(&inflate_memory, &source_buffer);
			char *raw;
			long long raw_size;

			raw_size = piece->length * 10;
			raw = new char [(size_t)raw_size];

			raw_size = inflate.read((unsigned char *)raw, raw_size);

			delete [] piece->file;
			piece->file = raw;
			piece->length = raw_size;
			}
		part[which] = piece->file;
		length[which] = piece->length;

		total_size += piece->length + 1;		// +1 because we might want to put a '\n' on the end
		which++;
		if (which > MAX_PARTS)
			exit(printf("Found more TREC assessments parts than allowed (max=%ld)\n", MAX_PARTS));
		}
	total_size += 1;							// +1 because we want a '\0' on the end
	delete file_stream;
	delete decompressor;
	delete instream_buffer;
	delete source;
	
	into = total = new char [(size_t)total_size];
	for (current = 0; current < which; current++)
		{
		memcpy(into, part[current], (size_t)length[current]);
		into += length[current];
		if (*(into - 1) != '\n')
			*into++ = '\n';
		delete [] part[current];
		}
	*into = '\0';
	return total;
	}
else
	return ANT_disk::read_entire_file(filename);
}

/*
	ANT_ASSESSMENT_TREC::READ()
	---------------------------
*/
ANT_relevant_document *ANT_assessment_TREC::read(char *filename, long long *judgements)
{
#ifndef FILENAME_INDEX
	char ***found, *pointer_to_document, **pointer_to_pointer_to_document;
	long missing_warned = FALSE;
#endif
char document[128];		// Assume document IDs are going to be less than 128 characters
char *file, **lines, **current;
long topic, subtopic, relevance, relevance_judgements;
long long lines_in_file;
ANT_relevant_document *current_assessment, *all_assessments;
long params;

/*
	load the assessment file into memory
*/
if ((file = read_entire_file(filename)) == NULL)
	return NULL;

lines = ANT_disk::buffer_to_list(file, &lines_in_file);

/*
	count the number of relevance judgements
*/
relevance_judgements = 0;
for (current = lines; *current != 0; current++)
	{
	params = sscanf(*current, "%ld %ld %127s %ld", &topic, &subtopic, document, &relevance);
	document[127] = '\0';
	if (params == 4)
		relevance_judgements++;
	else
		exit(printf("TREC assessment (line %lld) not quite right (expecting \"int int string int\" but got: \"%s\")\n", (long long)(current - lines), *current));
	}

/*
	allocate space for the assessments
*/
current_assessment = all_assessments = (ANT_relevant_document *)memory->malloc(sizeof(*all_assessments) * relevance_judgements);

/*
	generate the list of relevance judgements
*/
for (current = lines; *current != 0; current++)
	{
	params = sscanf(*current, "%ld %ld %127s %ld", &topic, &subtopic, document, &relevance);
	document[127] = '\0';
	if (params >= 4)
		{
		#ifdef FILENAME_INDEX
			current_assessment->docid = strnew(document);
		#else
			pointer_to_document = (char *)document;
			pointer_to_pointer_to_document = &pointer_to_document;
			found = (char ***)bsearch(&pointer_to_pointer_to_document, sorted_docid_list, (size_t)documents, sizeof(*sorted_docid_list), char_star_star_star_strcmp);
			if (found == NULL)
				{
				if (!missing_warned)
					printf("Warning: DOC '%s' is in the assessments, but not in the collection (now surpressing this warning)\n", document);
				/*
					Include it as a document with a _bad_ id that would never
					exist in the collection

					So, we can't just drop assessments for documents that aren't in the collection becasuse of spam filtering.
					If we drop a relevant document in the spam filter then that should not affect the ideal gain vector in DCG
					so each must have a unique id and be un-findable.  We achieve this by hashing the document's (TREC) ID and
					making that negative so that it does not clash with true internal docids.

					Matt Crane made did this.  It might result in a hash collision and so I've asked him to fix it.
				*/
				current_assessment->docid = -((long long)ANT_random_hash_24(document, strlen(document)));
				missing_warned = TRUE;
				}
			else
				current_assessment->docid = *found - docid_list;		// the position in the list of documents is the internal docid used for computing precision
		#endif

		current_assessment->topic = topic;
		current_assessment->subtopic = subtopic;
		current_assessment->document_length = relevance;
		current_assessment->relevant_characters = relevance;
		current_assessment->passage_list = NULL;
		current_assessment->number_of_relevant_passages = 0;
		current_assessment->best_entry_point = 0;
		current_assessment++;
		}
	}

/*
	clean up
*/
delete [] file;
delete [] lines;

/*
	and return
*/
*judgements = relevance_judgements;
return all_assessments;
}

