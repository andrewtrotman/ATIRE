/*
	GP_INDIVIDUAL.C
	---------------
*/

#include "gp_individual.h"
#include "gp_individual_node.h"

/*
	ANT_GP_INDIVIDUAL::ANT_GP_INDIVIDUAL()
	--------------------------------------
*/
ANT_gp_individual::ANT_gp_individual()
{
average_precision = -1;
function = new ANT_gp_individual_node();
}

/*
	ANT_GP_INDIVIDUAL::~ANT_GP_INDIVIDUAL()
	---------------------------------------
*/
ANT_gp_individual::~ANT_gp_individual()
{
delete function;
}

/*
	ANT_GP_INDIVIDUAL::EVALUATE()
	-----------------------------
*/
double ANT_gp_individual::evaluate(void)
{
double ans;

ans = function->eval();

return ans;
}