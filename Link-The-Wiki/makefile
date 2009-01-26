#
#Written (w) 2008 by Andrew Trotman, University of Otago
#

CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi -DHASHER=1 -DHEADER_HASHER=1  /O2 
CC = cl

all : link_extract.exe link_extract_pass2.exe link_index.exe link_index_merge.exe link_this.exe

.c.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

disk.obj : disk.c
disk_internams.obj : disk_internals.c

link_extract.exe : disk.obj disk_internals.obj link_extract.obj
	$(CC) $(CFLAGS) link_extract.obj disk.obj disk_internals.obj /Fe$@

link_extract_pass2.exe : disk.obj disk_internals.obj link_extract_pass2.obj
	$(CC) $(CFLAGS) link_extract_pass2.obj disk.obj disk_internals.obj /Fe$@

link_index.exe : disk.obj disk_internals.obj link_index.obj
	$(CC) $(CFLAGS) link_index.obj disk.obj disk_internals.obj /Fe$@

link_index_merge.exe : link_index_merge.obj
	$(CC) $(CFLAGS) link_index_merge.obj /Fe$@

link_this.exe : disk.obj disk_internals.obj link_this.obj
	$(CC) $(CFLAGS) link_this.obj disk.obj disk_internals.obj /Fe$@

clean :
	del *.obj *.exe *.ilk *.pdb *.suo *.pdb
