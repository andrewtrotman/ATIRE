#
# Makefile for ATIRE/Edit
#
.SILENT:

OBJECTS = 								\
	main.obj							\
	canvas.obj							\
	memory_file_line.obj				\
	memory_file_line_iterator.obj		\
	disk.obj							\
	file_internals.obj					\
	resources\resources.res

main.exe : $(OBJECTS)
	$(CXX) $(CXXFLAGS) /nologo /Zi -Femain.exe  $(OBJECTS) /link /fixed:no user32.lib gdi32.lib winmm.lib comdlg32.lib


CFLAGS = $(CFLAGS) /nologo /Zi /c /Tp

.c.obj:
	$(CC) $(CFLAGS) $< /Fo$@

{..\source\}.c.obj:
	$(CC) $(CFLAGS) $< /Fo$@

resources/resources.res : resources/resources.rc
	rc resources/resources.rc
	

clean :
	del $(OBJECTS) *.ilk *.pdb *.bak main.exe /s