ALL = bitstream.obj \
 compress_elias_gamma.obj \
 compress_elias_delta.obj \
 compress_golomb.obj \
 compress_simple9.obj \
 compress_relative10.obj \
 compress_carryover12.obj \
 compress_variable_byte.obj \
 compress_none.obj \
 compression_factory.obj \
 compress_sigma.obj \
 stats.obj \
 test_compression.obj \
 maths.obj

CFLAGS = /W4 /Zi /TP
all : nexi.exe test_compression.exe

nexi.exe : nexi.c ctypes.c string_pair.h parser.c
	$(CC) /W4 -D _CRT_SECURE_NO_WARNINGS /Zi /TP nexi.c ctypes.c parser.c

test_compression.exe : $(ALL)
	$(CC) /W4 /Zi $(ALL) /Fetest_compression.exe /link /fixed:no


clean :
	del $(ALL) test_compression.exe
