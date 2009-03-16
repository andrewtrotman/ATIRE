/*
	GP_INDIVIDUAL_NODE.C
	--------------------
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gp_individual_node.h"

using namespace std;

/*              + - * / L Q 0 1 2 3 4 5 6 7 8 9 1  F */
static long arity[] = {2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,-1};
static char *op_names  = "+1*/LQ0123456789XF";

/*
	ANT_GP_INDIVIDUAL_NODE::ANT_GP_INDIVIDUAL_NODE()
	------------------------------------------------
*/
ANT_gp_individual_node::ANT_gp_individual_node()
{
parameter1 = parameter2 = NULL;
constant = 0;
opcode = new_opcode();

if (opcode == FINAL)
	puts("GP construction went wrong!");
else if (opcode >= CONST0 && opcode <= CONST10)
	constant = (double)(opcode - CONST0) / 10.0;
else if (opcode == SQRT || opcode == LOG)
	parameter1 = new ANT_gp_individual_node;
else
	{
	parameter1 = new ANT_gp_individual_node;
	parameter2 = new ANT_gp_individual_node;
	}
}

/*
	ANT_GP_INDIVIDUAL_NODE::ANT_GP_INDIVIDUAL_NODE()
	------------------------------------------------
*/
ANT_gp_individual_node::ANT_gp_individual_node(ANT_gp_individual_node *old)
{
opcode = old->opcode;
constant = old->constant;
parameter1 = parameter2 = NULL;
if (old->parameter1 != NULL)
	parameter1 = new ANT_gp_individual_node(old->parameter1);
if (old->parameter2 != NULL)
	parameter2 = new ANT_gp_individual_node(old->parameter2);
}

/*
	ANT_GP_INDIVIDUAL_NODE::~ANT_GP_INDIVIDUAL_NODE()
	-------------------------------------------------
*/
ANT_gp_individual_node::~ANT_gp_individual_node()
{
delete parameter1;
delete parameter2;
}

/*
	ANT_GP_INDIVIDUAL_NODE::NEW_OPCODE()
	------------------------------------
*/
long ANT_gp_individual_node::new_opcode(void) 
{
return (long)((double)rand() / (double)(RAND_MAX + 1.0) * (double)FINAL);
}

/*
	ANT_GP_INDIVIDUAL::COPY()
	-------------------------
*/
ANT_gp_individual_node *ANT_gp_individual_node::copy(void)
{
return new ANT_gp_individual_node(this);
}

/*
	ANT_GP_INDIVIDUAL_NODE::SELECT_CHILD()
	--------------------------------------
*/
ANT_gp_individual_node *ANT_gp_individual_node::select_child(long *node)
{
ANT_gp_individual_node *answer;

if (*node < 0)
	answer = NULL;
else if (*node == 0)
	answer = this;
else
	{
	answer = NULL;
	(*node)--;
	if (parameter1 != NULL)
		answer = parameter1->select_child(node);
	if (answer == NULL && parameter2 != NULL)
		answer = parameter2->select_child(node);
	}
return answer;
}

/*
	ANT_GP_INDIVIDUAL_NODE::MUTATE()
	--------------------------------
*/
ANT_gp_individual_node *ANT_gp_individual_node::mutate(long *node)
{
ANT_gp_individual_node *who;
long old_arity, new_arity;

who = select_child(node);
old_arity = arity[who->opcode];

printf("replace:%c (arity:%d) with:", op_names[who->opcode], old_arity);

who->opcode = new_opcode();
new_arity = arity[who->opcode];

printf("%c (arity:%d)\n", op_names[who->opcode], new_arity);

/*
	Special case for handling constants
*/
if (who->opcode >= CONST0 && who->opcode <= CONST10)
	who->constant = (double)(who->opcode - CONST0) / 10.0;

/*
	If arities match then we're done
*/

if (new_arity == old_arity)
	return this;
/*
	Deal with parameter number (arity) mismatch
*/
if (new_arity < 0)
	puts("GP mutation went wrong!");
else if (new_arity == 0)
	{
	delete who->parameter1;
	delete who->parameter2;
	who->parameter1 = who->parameter2 = NULL;
	}
else if (new_arity == 1)
	{
	if (who->parameter1 == NULL)
		who->parameter1 = new ANT_gp_individual_node;
	delete who->parameter2;
	who->parameter2 = NULL;
	}
else if (new_arity == 2)
	{
	if (who->parameter1 == NULL)
		who->parameter1 = new ANT_gp_individual_node;
	if (who->parameter2 == NULL)
		who->parameter2 = new ANT_gp_individual_node;
	}

return this;
}

/*
	ANT_GP_INDIVIDUAL_NODE::CROSSOVER()
	-----------------------------------
*/
ANT_gp_individual_node *ANT_gp_individual_node::crossover(long *node, ANT_gp_individual_node *brother, long *node_in_brother)
{
ANT_gp_individual_node *my_node, *his_node;

my_node = select_child(node);
his_node = brother->select_child(node_in_brother);

delete my_node->parameter1;
delete my_node->parameter2;

my_node->opcode = his_node->opcode;
my_node->constant = his_node->constant;

my_node->parameter1 = my_node->parameter2 = NULL;

if (his_node->parameter1 != NULL)
	my_node->parameter1 = his_node->parameter1->copy();

if (his_node->parameter2 != NULL)
	my_node->parameter2 = his_node->parameter2->copy();

return this;
}


/*
	ANT_GP_INDIVIDUAL_NODE::GET_DEPTH()
	-----------------------------------
*/
long ANT_gp_individual_node::get_depth(void)
{
long left, right;

left = right = 0;
if (parameter1 != NULL)
	left = parameter1->get_depth();
if (parameter2 != NULL)
	right = parameter2->get_depth();

return left + right + 1;
}

/*
	ANT_GP_INDIVIDUAL_NODE::EVAL()
	------------------------------
*/
double ANT_gp_individual_node::eval(void)
{
double rhs;

switch (opcode)
	{
	case PLUS:
		return parameter1->eval() + parameter2->eval();
	case MINUS:
		return parameter1->eval() - parameter2->eval();
	case TIMES:
		return parameter1->eval() * parameter2->eval();
	case DIVIDE:
		{
		rhs = parameter2->eval();
		return rhs == 0 ? 0 : parameter1->eval() / rhs;
		}
	case SQRT:
		return sqrt(fabs(parameter1->eval()));
	case LOG:
		{
		rhs = fabs(parameter1->eval());
		return rhs == 0 ? 0 : log(rhs);
		}
	case CONST0:
	case CONST1:
	case CONST2:
	case CONST3:
	case CONST4:
	case CONST5:
	case CONST6:
	case CONST7:
	case CONST8:
	case CONST9:
	case CONST10:
		return constant;
	default:
		return 0.0;
	}
}

/*
	ANT_GP_INDIVIDUAL_NODE::TEXT_RENDER()
	-------------------------------------
*/
char *ANT_gp_individual_node::text_render(ostrstream &outstream)
{
switch (opcode)
	{
	case PLUS:
		outstream << "(";
		parameter1->text_render(outstream);
		outstream << ")+(";
		parameter2->text_render(outstream);
		outstream << ")";
		break;
	case MINUS:
		outstream << "(";
		parameter1->text_render(outstream);
		outstream << ")-(";
		parameter2->text_render(outstream);
		outstream << ")";
		break;
	case TIMES:
		outstream << "(";
		parameter1->text_render(outstream);
		outstream << ")*(";
		parameter2->text_render(outstream);
		outstream << ")";
		break;
	case DIVIDE:
		outstream << "(";
		parameter1->text_render(outstream);
		outstream << ")/(";
		parameter2->text_render(outstream);
		outstream << ")";
		break;
	case SQRT:
		outstream << "sqrt(fabs(";
		parameter1->text_render(outstream);
		outstream << "))";
		break;
	case LOG:
		outstream << "log(fabs(";
		parameter1->text_render(outstream);
		outstream << "))";
		break;
	case CONST0:
	case CONST1:
	case CONST2:
	case CONST3:
	case CONST4:
	case CONST5:
	case CONST6:
	case CONST7:
	case CONST8:
	case CONST9:
	case CONST10:
		outstream << constant;
		break;
	default:
		outstream << "UnknownOpcode";
		break;
	}
return "";
}

