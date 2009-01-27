/*
	GP.C
	----
*/

class ANT_gp
{
private:
	double crossover;
	double mutation;
	ANT_gp_individual *population1[POPULATION_SIZE];
	ANT_gp_individual *population2[POPULATION_SIZE];
	ANT_gp_individual **current, **next;

public:
	ANT_gp()
	virtual ~ANT_gp();
} ;

/*
	ANT_GP::ANT_GP()
	----------------
*/
ANT_gp::ANT_gp(double mutation, double crossover)
{
long individual;

this->mutation = mutation;
this->crossover = crossover;

next = population2;
current = population1;

for (individual = 0; individual < POPULATION_SIZE; individual++)
	{
	population1[individual] = new ANT_gp_individual;
	population2[individual] = NULL;
	}
}

/*
	ANT_GP::~ANT_GP()
	-----------------
*/
ANT_gp::~ANT_gp()
{
}

