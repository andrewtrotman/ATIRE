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
long seperators = 0;
char buffer[1024];		// look at the first 1KB of the file

if ((fp = fopen(filename, "r")) == NULL)
	return NULL;

while (fgets(buffer, sizeof(buffer), fp) != 0)
	if ((seperators = strcountchr(buffer, ' ')) != 0)
		break;
fclose(fp);

if (seperators == 1)
	factory = new ANT_assessment_ANT();
else if (seperators >= 4)
	factory = new ANT_assessment_INEX();

factory->copy(this);

return factory->read(filename, reldocs);
}
