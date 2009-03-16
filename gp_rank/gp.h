
/*
	GP.H
	----
*/

#ifndef __GP_H__
#define __GP_H__

#define POPULATION_SIZE 100

class ANT_gp_individual;

class ANT_gp
{
private:
	double crossover;
	double mutation;
	ANT_gp_individual *population1[POPULATION_SIZE];
	ANT_gp_individual *population2[POPULATION_SIZE];
	ANT_gp_individual **current, **next;

public:
	ANT_gp(double mutation, double crossover);		// reproduction = 1-(mutation+crossover)
	virtual ~ANT_gp();
} ;


#endif __GP_H__
