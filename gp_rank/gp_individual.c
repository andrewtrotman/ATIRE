/*
	GP_INDIVIDUAL.C
	---------------
*/

class ANT_gp_individual
{
public:
	ANT_gp_individual_node *function;
	double average_precision;

public:
	ANT_gp_individual();
	~ANT_gp_individual();

	double evaluate(void);
} ;

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

ans = function->evaluate();

return ans;
}