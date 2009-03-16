/*
	GP_MAIN.C
	---------
*/
#include <stdio.h>
#include <time.h>
#include <strstream>
#include "gp_individual_node.h"

using namespace std;

void text_render(ANT_gp_individual_node *func, char *message)
{
double ans;
ostrstream string;

func->text_render(string);
ans = func->eval();
string << ends;
printf("%s %f=%s\n", message, ans, string.str());
}

/*
	MAIN()
	------
*/
int main(void)
{
ANT_gp_individual_node *func, *func2;
long which, depth, node_to_alter, node_to_alter2;

srand((unsigned)time(NULL));

for (which = 0; which < 10; which++)
	{
	ostrstream string;

	func = new ANT_gp_individual_node;
	text_render(func, "Before1:");

	func2 = new ANT_gp_individual_node;
	text_render(func2, "Before2:");

/*
	MUTATION
	should probably copy first, then mutate

	depth = func->get_depth();
	depth--;
	node_to_alter = (long)((double)rand() / (double)(RAND_MAX + 1.0) * (double)depth);
	func->mutate(&node_to_alter);
*/

/*
	REPRODUCTION
	func = func->copy();
*/
/*
	CROSSOVER
*/
	depth = func->get_depth();
	depth--;
	node_to_alter = (long)((double)rand() / (double)(RAND_MAX + 1.0) * (double)depth);
	depth = func2->get_depth();
	depth--;
	node_to_alter2 = (long)((double)rand() / (double)(RAND_MAX + 1.0) * (double)depth);

	func->crossover(&node_to_alter, func2, &node_to_alter2);
/**/

	text_render(func, " After:");
	puts("");

	delete func;
	}
}

