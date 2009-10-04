WINDOWS_LIBS = user32.lib advapi32.lib kernel32.lib shlwapi.lib

main.exe : main.c instream_file.c instream_deflate.c
	cl /W3 /Zi -D_CRT_SECURE_NO_WARNINGS /Tp main.c /Tp instream_file.c /Tp instream_deflate.c -I c:\libarchive\zlib\zlib-1.2.3 c:\libarchive\zlib\zlib-1.2.3\zlib.lib ..\bin\file.obj ..\bin\memory.obj ..\bin\file_internals.obj $(WINDOWS_LIBS)

tar.exe : tar.c deflate.c
	cl /W3 /Zi /Tp tar.c deflate.c
