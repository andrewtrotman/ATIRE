ALL = bitstream.obj \
 compress_elias_gamma.obj \
 compress_elias_delta.obj \
 compress_golomb.obj \
 compress_simple9.obj \
 test_compression.obj \
 maths.obj

CFLAGS = /W3 /Zi /TP

test_compression.exe : $(ALL)
	$(CC) /W3 /Zi $(ALL) /Fetest_compression.exe


clean :
	del $(ALL) test_compression.exe
