#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <libgen.h> // basename()

#include <map>
#include <string>

#include "../../source/disk.h"

#define MAX_STR_LEN 1024

char doc_title[1024];

using namespace std;

/*
	GET_DOC_ID_2009()
	------------
*/
inline static long get_doc_id_2009(char *filename)
{
   // NOTE: this works on the filename, not the file contents
   return atol(basename(filename));
}

int main(int argc, char *argv[]) {
	
	/* Create the lookup structure. */
	ANT_disk disk;
	char *file, *start, *end, *from, *filename;
	long param, file_number, current_docid;
	int max_length = 0;
	
	map<string, int> lookup_structure;
	
	if (argc < 2)
		exit(printf("Usage:%s <filespec> ...\n", argv[0]));
	
	file_number = 1;
	for (param = 1; param < argc; param++)
	{
           filename = disk.get_first_filename(argv[param]);
           file = disk.read_entire_file(filename);
           while (file != NULL)
              {
                 current_docid = get_doc_id_2009(filename);
                 from = file;

                 /* NOTE: 2009 Wikipedia collection uses <title>...</title> instead of the old <name>...</name> */
                 if ((from = strstr(from, "<header>")) != NULL) {
                    if ((start = strstr(from, "<title")) != NULL) {
                       if ((end = strstr(start, "</title>")) != NULL) {
                          start = strchr(start, '>') + 1;
                          strncpy(doc_title, start, end - start);
                          doc_title[end - start] = '\0';

                          
                          for (int ch=0; doc_title[ch] != '\0'; ch++) {
                             if (isspace(doc_title[ch])) {
                                doc_title[ch] = ' ';
                             }
                          }
					
                          printf("%d %s\n", current_docid, doc_title);
                       }
                    }
                    
                 }
			
                 if (file_number % 10 == 0)
                    fprintf(stderr, "Files processed:%d\n", file_number);
                 file_number++;
			
                 delete [] file;
                 filename = disk.get_next_filename();
                 file = disk.read_entire_file(filename);
              }
	}
	
	
	fprintf(stderr, "%s Completed\n", argv[0]);
	return 0;
}
