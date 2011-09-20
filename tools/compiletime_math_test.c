#include <cstdio>
#include <limits>
#include <limits.h>
#include <math.h>
#include <cassert>

#include "../source/fundamental_types.h"
#include "../source/maths.h"

typedef unsigned long long pregen_t;

int main(void)
{

assert(ANT_compiletime_int_max<uint8_t>::value == 255);
assert(ANT_compiletime_int_max<uint16_t>::value == 65535);
assert(ANT_compiletime_int_max<uint32_t>::value == 4294967295ULL);
assert(ANT_compiletime_int_max<uint64_t>::value == 18446744073709551615ULL);
assert(ANT_compiletime_int_max<int8_t>::value == 127);
assert(ANT_compiletime_int_max<int16_t>::value == 32767);
assert(ANT_compiletime_int_max<int32_t>::value == 2147483647ULL);
assert(ANT_compiletime_int_max<int64_t>::value == 9223372036854775807ULL);

assert(ANT_compiletime_ispowerof2<2>::value != 0);
assert(ANT_compiletime_ispowerof2<3>::value == 0);
assert(ANT_compiletime_ispowerof2<4>::value != 0);
assert(ANT_compiletime_ispowerof2<5>::value == 0);
assert(ANT_compiletime_ispowerof2<8>::value != 0);
assert(ANT_compiletime_ispowerof2<16>::value != 0);
assert(ANT_compiletime_ispowerof2<32>::value != 0);
assert(ANT_compiletime_ispowerof2<64>::value != 0);
assert(ANT_compiletime_ispowerof2<128>::value != 0);
assert(ANT_compiletime_ispowerof2<256>::value != 0);
assert(ANT_compiletime_ispowerof2<512>::value != 0);

assert((ANT_compiletime_pow<2, 8>::value == 256));
assert((ANT_compiletime_pow<3, 3>::value == 27));
assert((ANT_compiletime_pow<7, 1>::value == 7));
assert((ANT_compiletime_pow<23, 0>::value == 1));

//We can use this to compute the number of bytes in our integer types
assert((ANT_compiletime_int_floor_log_to_base<uint8_t, 256>::value == 1));
assert((ANT_compiletime_int_floor_log_to_base<uint16_t, 256>::value == 2));
assert((ANT_compiletime_int_floor_log_to_base<uint32_t, 256>::value == 4));
assert((ANT_compiletime_int_floor_log_to_base<uint64_t, 256>::value == 8));

//And the number of bits
assert((ANT_compiletime_int_floor_log_to_base<uint8_t, 2>::value == 8));
assert((ANT_compiletime_int_floor_log_to_base<uint16_t, 2>::value == 16));
assert((ANT_compiletime_int_floor_log_to_base<uint32_t, 2>::value == 32));
assert((ANT_compiletime_int_floor_log_to_base<uint64_t, 2>::value == 64));

//Signed types lose a bit of storage for the sign
assert((ANT_compiletime_int_floor_log_to_base<int8_t, 2>::value == 7));
assert((ANT_compiletime_int_floor_log_to_base<int16_t, 2>::value == 15));
assert((ANT_compiletime_int_floor_log_to_base<int32_t, 2>::value == 31));
assert((ANT_compiletime_int_floor_log_to_base<int64_t, 2>::value == 63));

//Bytes and bits both fit into integer types with no remainder:
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint8_t, 256>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint16_t, 256>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint32_t, 256>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint64_t, 256>::value == 0));

assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint8_t, 2>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint16_t, 2>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint32_t, 2>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint64_t, 2>::value == 0));

assert((ANT_compiletime_int_floor_log_to_base_has_remainder<int8_t, 2>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<int16_t, 2>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<int32_t, 2>::value == 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<int64_t, 2>::value == 0));

//But there remainders in other situations, for power-of-two bases:
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint16_t, 8>::value != 0));
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint16_t, 32768>::value != 0));

//And for any non-power-of-two base, since they can't possibly divide without remainder into a power of two:
assert((ANT_compiletime_int_floor_log_to_base_has_remainder<uint16_t, 7>::value != 0));

assert((ANT_compiletime_int_floor_log_to_base<uint16_t, 255>::value == 2));
assert((ANT_compiletime_int_floor_log_to_base<uint16_t, 257>::value == 1));

assert((ANT_compiletime_int_floor_log_to_base<uint64_t, 37>::value == 12));
assert((ANT_compiletime_int_floor_log_to_base<uint64_t, 70>::value == 10));
assert((ANT_compiletime_int_floor_log_to_base<uint64_t, 2>::value == 64));
assert((ANT_compiletime_int_floor_log_to_base<uint64_t, 32>::value == 12));
assert((ANT_compiletime_int_floor_log_to_base<uint64_t, 256>::value == 8));

assert((ANT_compiletime_int_floor_log_to_base<int16_t, 31>::value == 3));
assert((ANT_compiletime_int_floor_log_to_base<int16_t, 32>::value == 3));
assert((ANT_compiletime_int_floor_log_to_base<int16_t, 33>::value == 2));
assert((ANT_compiletime_int_floor_log_to_base<int16_t, 181>::value == 2));
assert((ANT_compiletime_int_floor_log_to_base<int16_t, 182>::value == 1));

assert((ANT_compiletime_int_floor_log_to_base_remainder<uint64_t, 256, ANT_compiletime_int_floor_log_to_base_has_remainder<uint64_t, 256>::value>::value == 1));

printf("Tests pass\n");

return 0;
}
