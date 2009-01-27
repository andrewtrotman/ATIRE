/*
	GP_INDIVIDUAL_NODE.C
	--------------------
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strstream>

using namespace std;

/*             + - * / l s 0 1 2 3 4 5 6 7 8 9 1  F */
long *arity = {2,2,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,-1};

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
	long new_opcode(void) { opcode = (long)((double)rand() / (double)(RAND_MAX + 1.0) * (double)FINAL); }
	long get_depth(void);
	ANT_gp_individual_node *select_child(long *node);

public:
	ANT_gp_individual_node();
	virtual ~ANT_gp_individual_node();

	double eval(void);
	char *text_render(ostrstream &outstream);

	ANT_gp_individual *copy(void);
	void mutate(long *node);
} ;

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
	ANT_GP_INDIVIDUAL::COPY()
	-------------------------
*/
ANT_gp_individual *ANT_gp_individual::copy(void)
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
		answer = parameter1->mutate(node);
	if (answer == NULL && parameter2 != NULL)
		answer = parameter2->mutate(node);
	}
return answer;
}

/*
	ANT_GP_INDIVIDUAL::MUTATE()
	---------------------------
*/
void ANT_gp_individual::mutate(long *node)
{
ANT_gp_individual_node *who;
long old_arity, new_arity;

who = select_child(node);
old_arity = arity[who->opcode];
who->opcode = new_opcode();
new_arity = arity[who->opcode];
if (new_arity == old_arity)
	return;
/*
	Deal with parameter number (arity) mismatch
*/
if (new_arity < 0)
	puts("GP mutation went wrong!");
else (new_arity == 0)
	{
	delete who->parameter1;
	delete who->parameter2;
	who->parameter1 = who->parameter2 = NULL;
	}
else if (new_arity == 1)
	{
	if (who->parameter1 == NULL)
		who->parameter1 = new ANT_gp_individual;
	delete who->parameter2;
	who->parameter2 = NULL;
	}
else if (new_arity == 2)
	{
	if (who->parameter1 == NULL)
		who->parameter1 = new ANT_gp_individual;
	if (who->parameter2 == NULL)
		who->parameter2 = new ANT_gp_individual;
	}
}

/*
	ANT_GP_INDIVIDUAL::GET_DEPTH()
	------------------------------
*/
long ANT_gp_individual::get_depth(void)
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


/*
	MAIN()
	------
*/
int main(void)
{
ANT_gp_individual_node *func;
long which;
double ans;

for (which = 0; which < 10; which++)
	{
	ostrstream string;

	func = new ANT_gp_individual_node;
	func->text_render(string);
	ans = func->eval();
	string << ends;
	printf("%f=%s\n", ans, string.str());
	delete func;
	}
}

