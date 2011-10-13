#
# Makefile for ATIRE/Edit
#
.SILENT:

OBJECTS = 							\
	main.obj							\
	source_parser.obj					\
	source_parser_c.obj					\
	canvas.obj							\
	memory_file_line.obj				\
	memory_file_line_iterator.obj		\
	token_colours.obj					\
	line.obj							\
	disk.obj							\
	file_internals.obj					\
	ctypes.obj							\
	resources\resources.res

main.exe : $(OBJECTS)
	$(CXX) $(CXXFLAGS) /nologo /Zi -Femain.exe  $(OBJECTS) /link /fixed:no user32.lib gdi32.lib winmm.lib comdlg32.lib shlwapi.lib kernel32.lib advapi32.lib shell32.lib ole32.lib comctl32.lib

CFLAGS = $(CFLAGS) /nologo /Zi /c /D_UNICODE /DUNICODE /Tp


.c.obj:
	$(CC) $(CFLAGS) $< /Fo$@

{..\source\}.c.obj:
	$(CC) $(CFLAGS) $< /Fo$@

resources/resources.res : resources/resources.rc
	rc resources/resources.rc
	

clean :
	del $(OBJECTS) *.ilk *.pdb *.bak main.exe /s
