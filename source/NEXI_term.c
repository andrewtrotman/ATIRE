/*
	NEXI_TERM.C
	-----------
*/
#include <stdio.h>
#include "NEXI_term.h"
#include "string_pair.h"

/*
	ANT_NEXI_TERM::TEXT_RENDER()
	----------------------------
*/
void ANT_NEXI_term::text_render(void)
{
ANT_NEXI_term *parent;

if (path.start != NULL)
	{
	printf("TAG:");
	parent = parent_path;
	while (parent != NULL)
		{
		parent->path.text_render();
		parent = parent->next;
		}
	path.text_render();
	printf(" ");
	}

printf("TERM:");

if (sign > 0)
	printf("+");
else if (sign < 0)
	printf("-");

term.text_render();
printf("\n");

if (next != NULL) 
	next->text_render();
}
