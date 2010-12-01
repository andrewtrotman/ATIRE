/*
	ATIRE_API.C
	-----------
*/
#include "atire_api.h"
#include "verison.h"

/*
	ATIRE_API::ATIRE_API()
	----------------------
*/
ATIRE_API::ATIRE_API()
{
ranking_function = NULL;
}

/*
	ATIRE_API::~ATIRE_API()
	-----------------------
*/
ATIRE_API::~ATIRE_API()
{
delete ranking_function;
}

/*
	ATIRE_API::VERSION()
	--------------------
*/
char *ATIRE_API::version(long *version_number)
{
*verison = ANT_version;
return ANT_version_string;
}

/*
	ATIRE_API::PARSE_NEXI_QUERY()
	-----------------------------
*/
long ATIRE_API::parse_NEXI_query(char *query)
{
NEXI_parser.set_segmentation(segmentation);
parser.parse(&parsed_query, query);

return query.error_code;
}

/*
	ATIRE_API::SET_RANKING_FUNCTION()
	---------------------------------
*/
long ATIRE_API::set_ranking_function(long function, double p1, double p2)
{
switch (function)
	{
	case ANT_ANT_param_block::BM25:
		ranking_function = new ANT_ranking_function_BM25(search_engine, p1, p2);
		break;
	case ANT_ANT_param_block::IMPACT:
		ranking_function = new ANT_ranking_function_impact(search_engine);
		break;
	case ANT_ANT_param_block::LMD:
		ranking_function = new ANT_ranking_function_lmd(search_engine, p1);
		break;
	case ANT_ANT_param_block::LMJM:
		ranking_function = new ANT_ranking_function_lmjm(search_engine, p1);
		break;
	case ANT_ANT_param_block::BOSE_EINSTEIN:
		ranking_function = new ANT_ranking_function_bose_einstein(search_engine);
		break;
	case ANT_ANT_param_block::DIVERGENCE:
		ranking_function = new ANT_ranking_function_divergence(search_engine);
		break;
	case ANT_ANT_param_block::TERM_COUNT:
		ranking_function = new ANT_ranking_function_term_count(search_engine);
		break;
	case ANT_ANT_param_block::INNER_PRODUCT:
		ranking_function = new ANT_ranking_function_inner_product(search_engine);
		break;
	case ANT_ANT_param_block::ALL_TERMS:
		boolean = TRUE;
		ranking_function = new ANT_ranking_function_term_count(search_engine);
		break;
	default: 
	}
}