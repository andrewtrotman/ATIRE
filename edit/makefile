#
# Makefile for ATIRE/Edit
#

OBJECTS = 								\
	main.obj							\
	canvas.obj							\
	memory_file_line.obj				\
	memory_file_line_iterator.obj		\
	disk.obj							\
	file_internals.obj					\
	resources\resources.res

main.exe : $(OBJECTS)
	$(CXX) $(CXXFLAGS) /Zi -Femain.exe  $(OBJECTS) /link /fixed:no user32.lib gdi32.lib winmm.lib comdlg32.lib


.c.obj:
	$(CC) $(CFLAGS) /Zi /c /Tp $< /Fo$@

{..\source\}.c.obj:
	$(CC) $(CFLAGS) /Zi /c /Tp $< /Fo$@

resources/resources.res : resources/resources.rc
	rc resources/resources.rc
	

clean :
	del $(OBJECTS) *.ilk *.pdb *.bak main.exe /s