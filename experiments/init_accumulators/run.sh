g++ -DPRINT_TIME_NO_CONVERSION -O3 -o init_accumulators init_accumulators.cpp \
../../source/str.h \
../../source/fundamental_types.h \
../../source/bitstring.h \
../../source/bitstring.c \
../../source/maths.c \
../../source/memory.c \
./mt19937-64.c \
-Wno-write-strings \
-DUSE_BITSTRING \
-DVARIABLE_ROUNDED_TO_BEST
