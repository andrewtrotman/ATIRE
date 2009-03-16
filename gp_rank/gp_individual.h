
/*
	GP_INDIVIDUAL.H
	---------------
*/

#ifndef __GP_INDIVIDUAL_H__
#define __GP_INDIVIDUAL_H__

class ANT_gp_individual_node;

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



#endif __GP_INDIVIDUAL_H__
