#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>

#include <map>
#include <string>

#include "../../source/disk.h"

#define MAX_STR_LEN 1024

char doc_title[1024];

using namespace std;

typedef struct {
   long id;
   char *title;
} article_info;

int article_info_compare(const void *a1, const void *a2) {
   const article_info *article1 = (const article_info*)a1;
   const article_info *article2 = (const article_info*)a2;

   return strcasecmp(article1->title, article2->title);
}

int main(int argc, char *argv[]) {
	
	ANT_disk disk;
	char *file, *start, *end, *from;
	long param, file_number, current_docid;
	int max_length = 0;
	
	if (argc < 2)
		exit(printf("Usage:%s <filespec>\n", argv[0]));

	file = disk.read_entire_file(disk.get_first_filename(argv[1]));
        
	long long num_lines_ll;
	int num_lines;
	char **lines = disk.buffer_to_list(file, &num_lines_ll);
	num_lines = (int)num_lines_ll;
	
	printf("%d\n", num_lines);
	
	//num_lines = 100;

	article_info *articles = (article_info*)malloc(sizeof *articles * (int)num_lines);

	int count = 0;
	for (int cur_line = 0; cur_line < num_lines; cur_line++) {
		articles[cur_line].id = atol(lines[cur_line]);
		bool found = false;
		for (int ch=0; lines[cur_line][ch] != '\0'; ch++) {
			if (lines[cur_line][ch] == ' ') {
				articles[cur_line].title = lines[cur_line] + ch + 1;
				found = true;
				break;
			}
		}
		if (!found) fprintf(stderr, "Invalid string \"%s\" on line %d.\n", lines[cur_line], cur_line);
		//articles[cur_line].title = strchr(lines[cur_line], ' ');
		//fprintf(stderr, "Read line with title [%s]\n", articles[cur_line].title);
		count++;
	}
	fprintf(stderr, "Processed %d lines.\n", count);

	qsort(articles, num_lines, sizeof *articles, article_info_compare);
	
	for (int cur_line = 0; cur_line < num_lines; cur_line++) {
		//fprintf(stderr, "ARTICLE [%s] has id %d\n", articles[cur_line].title, articles[cur_line].id);
	}
        
	/* Go through the log file. */
	
	FILE *infile = stdin, *outfile = stdout;
	
	char user_hash[MAX_STR_LEN+1], time[MAX_STR_LEN+1], method[MAX_STR_LEN+1],
	url[MAX_STR_LEN+1], protocol[MAX_STR_LEN+1];
	
	int status, bytes;
	
	regex_t url_regex;
	
	// FIXME: find out why this doesn't filter out trailing question marks.
	regcomp(&url_regex, "http://en.wikipedia.org/wiki/([^ ?]*)", REG_EXTENDED | REG_ICASE);
	
	regmatch_t matches[2];
	
	article_info search_article;
	search_article.id = 0;
	search_article.title = "";
	
	/*
	 Input format:
	 <hashed username> <HTTP method> <URL> <HTTP status> <bytes received> <timestamp (DD/Mon/YYYY:HH:MM:SS)> <original log filename>
	 Output format:
	 <hashed username> <URL> <Article ID> <timestamp (DD/Mon/YYYY:HH:MM:SS)>
	 */
	while (fscanf(infile, "%1024s %*s %1024s %*d %*d %1024s %*s\n",
				  user_hash, url, time) != EOF) {
		// Extract the article title. Ignore requests for non-articles.
		if (regexec(&url_regex, url, (sizeof matches / sizeof matches[0]), matches, 0) != REG_NOMATCH) {
			char *title = (char*)malloc(sizeof *title * (matches[1].rm_eo - matches[1].rm_so + 1));
			strncpy(title, (url + matches[1].rm_so), (matches[1].rm_eo - matches[1].rm_so));
			
			// Crude conversion from WP's URL-encoding to plain text.
			for (int ch = 0; title[ch] != '\0'; ch++) {
				if (title[ch] == '_') {
					title[ch] = ' ';
				}
			}

			search_article.title = title;
			article_info *bsearch_match = (article_info*)bsearch(&search_article, articles, num_lines, sizeof *articles, article_info_compare);
			
			printf("%s %s ", user_hash, url);
			
			if (bsearch_match != NULL) {
				printf("%d", bsearch_match->id);
			}else{
				printf("not_found");
			}
			
			printf(" %s\n", time);
			 
		}
		
	}
	
	fprintf(stderr, "%s Completed\n", argv[0]);
	return 0;
}
