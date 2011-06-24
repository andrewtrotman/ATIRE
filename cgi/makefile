
all : ant_cgi.exe ant_getdoc_cgi.exe


LIBS = WS2_32.lib 					\
	../bin/str.obj 				\
	../bin/channel_socket.obj 	\
	../bin/sockets.obj			\
	../bin/disk.obj				\
	../bin/atire_api_remote.obj	\
	../bin/file_internals.obj	



ant_cgi.exe : ant_cgi.c
	cl /MT -D_CRT_SECURE_NO_WARNINGS /W3 /Zi /Tp ant_cgi.c $(LIBS)

ant_getdoc_cgi.exe : ant_getdoc_cgi.c
	cl /MT -D_CRT_SECURE_NO_WARNINGS /W3 /Zi /Tp ant_getdoc_cgi.c $(LIBS)



clean :
	del *.obj *.exe *.ilk *.pdb
