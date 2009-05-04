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
 test_compression.obj \
 maths.obj

CFLAGS = /W4 /Zi /TP

test_compression.exe : $(ALL)
	$(CC) /W4 /Zi $(ALL) /Fetest_compression.exe /link /fixed:no


clean :
	del $(ALL) test_compression.exe
