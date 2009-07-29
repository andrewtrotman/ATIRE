/*
	ASSESSMENT_FACTORY.C
	--------------------
*/
#include <stdio.h>
#include "str.h"
#include "assessment_factory.h"
#include "assessment_ANT.h"
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
ANT_relevant_document *ANT_assessment_factory::read(char *filename, long long *reldocs)
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
else 
    exit(fprintf(stderr, "Unrecognised assessment format.\n"));

factory->copy(this);

return factory->read(filename, reldocs);
}
