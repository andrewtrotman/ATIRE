/*
	GP.C
	----
*/
#include <stdio.h>
#include "gp.h"
#include "gp_individual.h"

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

