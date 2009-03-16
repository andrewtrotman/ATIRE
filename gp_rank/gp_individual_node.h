/*
	GP_INDIVIDUAL_NODE.H
	--------------------
*/

#ifndef __GP_INDIVIDUAL_NODE_H__
#define __GP_INDIVIDUAL_NODE_H__

#include <strstream>

class ANT_gp_individual_node
{
private:
	enum  {PLUS, MINUS, TIMES, DIVIDE, LOG, SQRT,
			CONST0, CONST1, CONST2, CONST3, CONST4, CONST5,
			CONST6, CONST7, CONST8, CONST9, CONST10,
			FINAL} ;
public:
	ANT_gp_individual_node *parameter1, *parameter2;
	long opcode;
	double constant;

private:
	long new_opcode(void);
	ANT_gp_individual_node *select_child(long *node);
	ANT_gp_individual_node(ANT_gp_individual_node *old);

public:
	ANT_gp_individual_node();
	virtual ~ANT_gp_individual_node();

	long get_depth(void);

	ANT_gp_individual_node *copy(void);
	ANT_gp_individual_node *mutate(long *node);
	ANT_gp_individual_node *crossover(long *node, ANT_gp_individual_node *brother, long *node_in_brother);

	double eval(void);
	char *text_render(std::ostrstream &outstream);
} ;

#endif __GP_INDIVIDUAL_NODE_H__
