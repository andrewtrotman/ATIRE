/*
	ASSESSMENT_FACTORY.C
	--------------------
*/
#include <stdlib.h>
#include <stdio.h>
#include "str.h"
#include "assessment_factory.h"
#include "assessment_ANT.h"
#include "assessment_TREC.h"
#include "assessment_INEX.h"

/*
	ANT_ASSESSMENT_FACTORY::~ANT_ASSESSMENT_FACTORY()
	-------------------------------------------------
*/
ANT_assessment_factory::~ANT_assessment_factory()
{
delete factory;
}

/*
	ANT_ASSESSMENT_FACTORY::READ()
	------------------------------
*/
ANT_relevant_document *ANT_assessment_factory::read(char *filename, long long *judgements)
{
FILE *fp;
long separators = 0;
char buffer[1024];		// look at the first 1KB of the file

if ((fp = fopen(filename, "r")) == NULL)
	return NULL;

while (fgets(buffer, sizeof(buffer), fp) != 0)
	if ((separators = strcountchr(buffer, ' ')) != 0)
		break;
fclose(fp);

if (separators == 1)
	factory = new ANT_assessment_ANT();
else if (separators >= 4)
	factory = new ANT_assessment_INEX();
else if (separators == 3)
	factory = new ANT_assessment_TREC();
else
	exit(fprintf(stderr, "Unrecognised assessment format.\n"));

factory->copy(this);

return factory->read(filename, judgements);
}
