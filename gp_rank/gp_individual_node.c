/*
	GP_INDIVIDUAL_NODE.C
	--------------------
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <strstream>

using namespace std;

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

public:
	ANT_gp_individual_node();
	virtual ~ANT_gp_individual_node();

	double eval(void);
	char *text_render(ostrstream &outstream);
} ;

/*
	ANT_GP_INDIVIDUAL_NODE::ANT_GP_INDIVIDUAL_NODE()
	------------------------------------------------
*/
ANT_gp_individual_node::ANT_gp_individual_node()
{
parameter1 = parameter2 = NULL;
constant = 0;
opcode = 0;
double r;

r = rand();

opcode = (long)(r / (RAND_MAX + 1.0) * FINAL);
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
	ANT_GP_INDIVIDUAL_NODE::~ANT_GP_INDIVIDUAL_NODE()
	-------------------------------------------------
*/
ANT_gp_individual_node::~ANT_gp_individual_node()
{
delete parameter1;
delete parameter2;
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