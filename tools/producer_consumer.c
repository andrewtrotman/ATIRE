/*
	PRODUCER_CONSUMER.C
	-------------------
	Example producer / consumer model
*/
#ifdef _MSC_VER
	#include <windows.h>
#endif
#include <stdio.h>
#include "../source/critical_section.h"
#include "../source/semaphores.h"
#include "../source/threads.h"

#define BUFFER_SIZE 10

ANT_critical_section mutex;
ANT_semaphores fill_count(0, BUFFER_SIZE);
ANT_semaphores empty_count(BUFFER_SIZE, BUFFER_SIZE);

long insertion_point = 0;
long removal_point = 0;
long buffer[1024];
long buffer_length = BUFFER_SIZE;

/*
	PRODUCE_ITEM()
	--------------
*/
long produce_item(void)
{
static long val = 100;

return val++;
}

/*
	CONSUME_ITEM()
	--------------
*/
void consume_item(long item)
{
printf("%lld ", (long long)item);
}

/*
	PUT_ITEM_IN_BUFFER()
	--------------------
*/
void put_item_in_buffer(long value)
{
buffer[insertion_point] = value;
insertion_point = (insertion_point + 1) % buffer_length;
printf(" [%d] ", value);
}

/*
	GET_ITEM_FROM_BUFFER()
	----------------------
*/
long get_item_from_buffer(void)
{
long answer;

answer = buffer[removal_point];
removal_point = (removal_point + 1) % buffer_length;
return answer;
}

/*
	PRODUCER()
	----------
*/
void *producer(void *ignore)
{
long item;

ignore = NULL;			// be rid of the compiler warning.
for (;;)
	{
	item = produce_item();
	empty_count.enter();
	mutex.enter();
	put_item_in_buffer(item);
	mutex.leave();
	fill_count.leave();
	}
}

/*
	CONSUMER()
	----------
*/
void consumer(void)
{
long item = 0;

while (item < 200)
	{
	fill_count.enter();
	mutex.enter();
	item = get_item_from_buffer();
	mutex.leave();
	empty_count.leave();
	consume_item(item);
//	Sleep(1000);
	}
}

/*
	MAIN()
	------
*/
int main(void)
{
ANT_thread(producer, NULL);
consumer();

return 0;
}
