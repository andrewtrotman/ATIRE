#include <stdlib.h>

class spam
{
public:
	long x;

public:
	void something(void)
	{
//	#pragma omp flush (x)
	#pragma omp atomic
	x++;
	}
};


int main(int argc, char *argv[])
{
long lockable;
spam thing;

lockable = 0;
thing.x = 0;

#pragma omp flush (lockable)



thing.x = 0;

exit(0);

}