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
#include "snippet_tf.h"
#include "snippet_best_tag_tf.h"
#include "snippet_word_cloud.h"

/*
	ANT_SNIPPET_FACTORY::GET_SNIPPET_MAKER()
	----------------------------------------
*/
ANT_snippet *ANT_snippet_factory::get_snippet_maker(long type, long length_of_snippet, long length_of_longest_document, char *tag, ANT_search_engine *engine, ANT_stem *stemmer, long cloud_terms)
{
ANT_snippet *answer = NULL;

switch (type)
	{
	case SNIPPET_TITLE:
		answer = new (std::nothrow) ANT_snippet_tag(length_of_snippet, length_of_longest_document, engine, stemmer, tag);
		break;
	case SNIPPET_BEGINNING:
		answer = new (std::nothrow) ANT_snippet_beginning(length_of_snippet, length_of_longest_document, engine, stemmer, tag);
		break;
	case SNIPPET_TF:
		answer = new (std::nothrow) ANT_snippet_tf(length_of_snippet, length_of_longest_document, engine, stemmer);
		break;
	case SNIPPET_TFICF:
		answer = new (std::nothrow) ANT_snippet_tficf(length_of_snippet, length_of_longest_document, engine, stemmer);
		break;
	case SNIPPET_BEST_TFICF_TAG:
		answer = new (std::nothrow) ANT_snippet_best_tag(length_of_snippet, length_of_longest_document, engine, stemmer, tag);
		break;
	case SNIPPET_BEST_TF_TAG:
		answer = new (std::nothrow) ANT_snippet_best_tag_tf(length_of_snippet, length_of_longest_document, engine, stemmer, tag);
		break;
	case SNIPPET_WORD_CLOUD_KL:
		answer = new (std::nothrow) ANT_snippet_word_cloud(length_of_snippet, length_of_longest_document, engine, stemmer, cloud_terms);
		break;
	default:
		exit(printf("Unknown snippet algorithm, so cannot create a snippet generator\n"));
	}

return answer;
}