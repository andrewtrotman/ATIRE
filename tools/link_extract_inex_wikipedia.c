/*
	LINK_EXTRACT_INEX_WIKIPEDIA.C
	-----------------------------
	Extract the links from the INEX 2009 Wikipedia collection.  The output file
	is a <long><long> where the first long is the source and the second is the
	target document.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/str.h"
#include "../source/file.h"
#include "../source/memory.h"
#include "../source/instream_file.h"
#include "../source/instream_bz2.h"
#include "../source/instream_buffer.h"
#include "../source/directory_iterator_tar.h"
#include "../source/directory_iterator_multiple.h"
#include "../source/directory_iterator_object.h"


/*
	USAGE()
	-------
*/
int usage(char *exename)
{
printf("Usage:%s <outfile> <infile> ... <infile>\n", exename);
return 0;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
static char destination[1024 * 1024];
ANT_memory file_buffer(1024 * 1024);
ANT_instream_file *file_stream;
ANT_instream_bz2 *decompressor;
ANT_instream_buffer *instream_buffer;
ANT_directory_iterator_tar *source;
ANT_directory_iterator_multiple *disk;
ANT_directory_iterator_object file_object, *current_file;
ANT_file outfile;
long param, length;
char *document, *pos, *link, *slash;
long source_id, destination_id;
long documents_processed;
long broken_link;

if (argc < 3)
	exit(usage(argv[0]));

disk = new ANT_directory_iterator_multiple;
for (param = 2; param < argc; param++)
	{
	file_stream = new ANT_instream_file(&file_buffer, argv[param]);
	decompressor = new ANT_instream_bz2(&file_buffer, file_stream);
	instream_buffer = new ANT_instream_buffer(&file_buffer, decompressor);
	source = new ANT_directory_iterator_tar(instream_buffer, ANT_directory_iterator::READ_FILE);
	disk->add_iterator(source);
	}

outfile.open(argv[1], "wb");

documents_processed = 0;
for (current_file = disk->first(&file_object); current_file != NULL; current_file = disk->next(&file_object))
	{
	documents_processed++;
	if ((documents_processed % 1000) == 0)
		fprintf(stderr, "%ld\n", documents_processed);

	document = file_object.file;
	if ((slash = strrchr(file_object.filename, '/')) != NULL)
		{
		source_id = ANT_atol(slash + 1);
		while ((link = strstr(document, "<link")) != NULL)
			{
			if ((pos = strchr(link, '>')) == NULL)
				break;

			pos++;		// include the '>' in the extraction
			length = pos - link < sizeof(destination) - 1 ? pos - link : sizeof(destination) - 1;
			strncpy(destination, link, length);
			destination[length] = '\0';
			if (strstr(destination, "simple") != NULL)
				if ((slash = strchr(destination, '/')) != NULL)
					{
					broken_link = false;
					slash++;
					while (*slash != '/' && *slash != '\0')
						{
						if (!ANT_isdigit(*slash))
							broken_link = true;
						slash++;
						}
#ifdef NEVER
					if (broken_link)
						printf("BROKEN: %s -> %s\n", current_file->filename, destination);
					else
						printf("%s -> %s\n", current_file->filename, destination);
#endif
					
					if (!broken_link && *slash != '\0')
						{
						destination_id = ANT_atol(slash + 1);

						if (source_id != 0 && destination_id != 0)
							{
							outfile.write((unsigned char *)&source_id, sizeof(source_id));
							outfile.write((unsigned char *)&destination_id, sizeof(destination_id));
							}
#ifdef NEVER
						else
							printf("%ld -> %ld %s -> %s\n", source_id, destination_id, current_file->filename, destination);
#endif

						}
					}
			document = pos;
			}
		}
	delete [] current_file->file;
	delete [] current_file->filename;
	}
fprintf(stderr, "Done:%ld\n", documents_processed);
}
