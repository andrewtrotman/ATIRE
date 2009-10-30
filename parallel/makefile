CC = cl
CFLAGS = /Od /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi

.c.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

all : pc.exe

ALL= producer_consumer.obj semaphore.obj critical_section.obj threads.obj

pc.exe : $(ALL)
	cl $(ALL) /Fepc.exe


