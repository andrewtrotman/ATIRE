#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../source/compression_factory.h"
#include "../source/compress.h"


int main(void)
{

ANT_compression_factory factory;

unsigned char buffer[100];

int len;
ANT_compressable_integer a = 2300770;
factory.set_scheme(ANT_compression_factory::VARIABLE_BYTE);
len = factory.compress(buffer, 5, &a, (long long)1);
printf("len: %d\n", len);
return 0;
}
