CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /Zi 
CC = cl

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
 maths.obj \
 ctypes.obj \
 parser.obj \
 plugin_manager.obj\
 nexi_term.obj 

all : nexi.exe test_compression.exe

nexi.exe : $(ALL)
	$(CC) /W4 -D_CRT_SECURE_NO_WARNINGS /Zi $(ALL) /Tp nexi.c /Fenexi.exe

test_compression.exe : $(ALL)
	$(CC) /W4 -D_CRT_SECURE_NO_WARNINGS /Zi $(ALL) /Tp test_compression.c /Fetest_compression.exe /link /fixed:no

.c.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

clean :
	del $(ALL) test_compression.exe nexi.exe
