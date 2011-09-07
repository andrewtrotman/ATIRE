/*
	SNIPPET_FACTORY.C
	-----------------
*/
#include <stdlib.h>
#include <stdio.h>
#include <new>
#include "snippet_factory.h"
#include "snippet_beginning.h"
#include "snippet_tag.h"

/*
	ANT_SNIPPET_FACTORY::GET_SNIPPET_MAKER()
	----------------------------------------
*/
ANT_snippet *ANT_snippet_factory::get_snippet_maker(long type, long length, char *tag)
{
ANT_snippet *answer = NULL;

switch (type)
	{
	case SNIPPET_TITLE:
		answer = new (std::nothrow) ANT_snippet_tag(length, tag);
		break;
	case SNIPPET_BEGINNING:
		answer = new (std::nothrow) ANT_snippet_beginning(length, tag);
		break;
	default:
		exit(printf("Unknown snippet algorithm, so cannot create a snippet generator\n"));
	}

return answer;
}