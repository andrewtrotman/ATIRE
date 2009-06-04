#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>

#include <map>
#include <string>

#include "../../source/disk.h"

#define MAX_STR_LEN 1024

using namespace std;

typedef struct {
   long source_id;
   long target_id;
   char *anchor_text;
} link_info;

typedef struct {
   long article_id;
   char *user_hash;
} window_entry;

/*
  Compare by target ID first, then by source ID.
 */
int link_info_compare(const void *l1, const void *l2) {
   const link_info *link1 = (const link_info*)l1;
   const link_info *link2 = (const link_info*)l2;

   long target_diff = link1->target_id - link2->target_id;
   if (target_diff != 0) return target_diff;

   long source_diff = link1->source_id - link2->source_id;
   return source_diff;
}

int main(int argc, char *argv[]) {
	
	ANT_disk disk;
	char *file, *start, *end, *from;
	long param, file_number, current_docid;
	int max_length = 0;
	
	if (argc < 2)
		exit(printf("Usage:%s <filespec>\n", argv[0]));

        fprintf(stderr, "Loading file...\n");

	file = disk.read_entire_file(disk.get_first_filename(argv[1]));
        
	long long num_lines_ll;
	int num_lines;
	char **lines = disk.buffer_to_list(file, &num_lines_ll);
	num_lines = (int)num_lines_ll;

	link_info *links = (link_info*)malloc(sizeof *links * num_lines);

	int count = 0;
	for (int cur_line = 0; cur_line < num_lines; cur_line++) {
           int len_read;
           sscanf(lines[cur_line], "%d:%d:%n",
                  &(links[cur_line].source_id),
                  &(links[cur_line].target_id),
                  &len_read);

           links[cur_line].anchor_text = strdup(lines[cur_line] + len_read);
	}

        fprintf(stderr, "Sorting...\n");

	qsort(links, num_lines, sizeof *links, link_info_compare);
       
        fprintf(stderr, "Processing log...\n");

	/* Go through the log file. */
	
	FILE *infile = stdin, *outfile = stdout;
	
	char user_hash[MAX_STR_LEN+1], time[MAX_STR_LEN+1], method[MAX_STR_LEN+1],
	url[MAX_STR_LEN+1], protocol[MAX_STR_LEN+1];
        long article_id;
        char article_id_str[MAX_STR_LEN+1];
	
	int status, bytes;
	
	link_info search_link;
        search_link.source_id = search_link.target_id = 0;
        search_link.anchor_text = "";

        // Create a "window" to store the N most recent log entries processed.
        #define WINDOW_SIZE 20
        window_entry window[WINDOW_SIZE];
        int window_first = 0;
        int window_size_used = 0;
	
	/*
	 Input format:
	 <hashed username> <URL> <Article ID> <timestamp (DD/Mon/YYYY:HH:MM:SS)>
	 Output format:
	 <hashed username> <URL> <Article ID> <timestamp (DD/Mon/YYYY:HH:MM:SS)> <possible link source and anchor>
	 */
	while (fscanf(infile, "%1024s %1024s %1024s %1024s\n",
                      user_hash, url, article_id_str, time) != EOF) {

           if (strcmp(article_id_str, "not_found") == 0) continue;

           article_id = atol(article_id_str);

           printf("%s %s %ld %s ", user_hash, url, article_id, time);

           
           // Search in the window for accesses to articles which link to this one.
           // FIXME: make this search them in reverse order of access time.
           bool found = false;
           for (int i = 0; i < window_size_used; i++) {
              search_link.source_id = window[i].article_id;
              search_link.target_id = article_id;

              /* We seem to be picking up a lot of articles linking to themselves (perhaps a bug in the link-extraction code)
                 so we'll ignore those cases, since they're more likely to be page-refreshes than link-clicks.
              */
              if (search_link.source_id == search_link.target_id) continue;
              
              link_info *bsearch_match = (link_info*)bsearch(&search_link, links, num_lines, sizeof *links, link_info_compare);

              if (bsearch_match != NULL) {
                 // Found one!

                 // First, though, make sure the username was the same for both accesses.
                 // (This isn't checked for in link_info_compare because I wanted to use the same comparator for the qsort() call.)

                 if (strcmp(window[i].user_hash, user_hash) == 0) {
                    printf("(probable link: from article ID %ld with anchor text \"%s\")", bsearch_match->source_id, bsearch_match->anchor_text);
                    found = true;
                    break;
                 }
              }
           }

           if (!found) printf("(no link found in the last %d entries)", WINDOW_SIZE);

           printf("\n");

           // Add the entry to the window.
           // If it's full, have it replace the least recent one that's currently in there.
           window_entry new_entry;
           new_entry.article_id = article_id;
           new_entry.user_hash = strdup(user_hash);

           if (window_size_used < WINDOW_SIZE) {
              window[window_size_used++] = new_entry;
           }else{
              window[(window_first + window_size_used) % WINDOW_SIZE] = new_entry;
              window_first = (window_first + 1) % WINDOW_SIZE;
           }
        }
	
	fprintf(stderr, "%s Completed\n", argv[0]);
	return 0;
}
