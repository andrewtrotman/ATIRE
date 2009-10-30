/*
semaphore mutex = 1
semaphore fillCount = 0
semaphore emptyCount = BUFFER_SIZE

procedure producer() {
    while (true) {
        item = produceItem()
        down(emptyCount)
        down(mutex)
        putItemIntoBuffer(item)
        up(mutex)
        up(fillCount)
    }
    up(fillCount) //the consumer may not finish before the producer.
 }

procedure consumer() {
    while (true) {
        down(fillCount)
        down(mutex)
        item = removeItemFromBuffer()
        up(mutex)
        up(emptyCount)
        consumeItem(item)
    }
}
*/
#include <windows.h>
#include <stdio.h>
#include "critical_section.h"
#include "semaphore.h"
#include "threads.h"

#define BUFFER_SIZE 10

ANT_critical_section mutex;
ANT_semaphore fill_count(0, BUFFER_SIZE);
ANT_semaphore empty_count(BUFFER_SIZE, BUFFER_SIZE);

long insertion_point = 0;
long removal_point = 0;
long buffer[1024];
long buffer_length = BUFFER_SIZE;

long produce_item(void)
{
static long val = 100;

return val++;
}
void consume_item(long item)
{
printf("%lld ", (long long)item);
}

void put_item_in_buffer(long value)
{
buffer[insertion_point] = value;
insertion_point = (insertion_point + 1) % buffer_length;
printf(" [%d] ", value);
}

long get_item_from_buffer(void)
{
long answer;

answer = buffer[removal_point];
removal_point = (removal_point + 1) % buffer_length;
return answer;
}

void *producer(void *ignore)
{
long item;

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

int main(void)
{
ANT_thread(producer, NULL);
consumer();

return 0;
}