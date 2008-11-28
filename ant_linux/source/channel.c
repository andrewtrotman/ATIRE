/*
	CHANNEL.C
	---------
*/
#include <windows.h>
#include <stdio.h>

class ANT_channel
{
private:
	enum {NODATA, DATA};

public:
	volatile long flip;
	volatile char *datum;

private:
	void spin(void);
public:
	ANT_channel();
	~ANT_channel();

	ANT_channel *operator<< (char * datum);
	ANT_channel *operator>>(char * datum);
} ;

/*
	ANT_CHANNEL::ANT_CHANNEL()
	--------------------------
*/
ANT_channel::ANT_channel()
{
flip = NODATA;
}

/*
	ANT_CHANNEL::~ANT_CHANNEL()
	---------------------------
*/
ANT_channel::~ANT_channel()
{
}

/*
	ANT_CHANNEL::SPIN()
	-------------------
*/
void ANT_channel::spin(void)
{
Sleep(0);
}

/*
	ANT_CHANNEL::OPERATOR<<()
	-------------------------
*/
ANT_channel *ANT_channel::operator<<(char *what)
{
long more;

#pragma omp critical
	{
	datum = what;
	flip = DATA;
	while (more == DATA)
		{
		spin();
		more = flip;
		}
	}
return this;
}

/*
	ANT_CHANNEL::OPERATOR>>()
	-------------------------
*/
ANT_channel *ANT_channel::operator>>(char *answer)
{
#pragma omp critical
	{
	while (flip == NODATA)
		spin();
	answer = (char *)datum;
	datum = NULL;
	flip = NODATA;
	}

return this;
}
