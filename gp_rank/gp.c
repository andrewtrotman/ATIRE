/*
	GP.C
	----
*/

class ANT_gp
{
private:
	double crossover;
	double mutation;
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
this->mutation = mutation;
this->crossover = crossover;
}

/*
	ANT_GP::~ANT_GP()
	-----------------
*/
ANT_gp::~ANT_gp()
{
}

