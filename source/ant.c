#include <stdio.h>
#include "memory.h"
#include "search_engine.h"

int main(int argc, char *argv[])
{
ANT_memory memory;

puts("Start");
ANT_search_engine search_engine(&memory);
puts("End");

return 0;
}
