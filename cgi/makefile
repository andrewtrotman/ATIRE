
all : ant_cgi.exe ant_getdoc_cgi.exe


LIBS = WS2_32.lib 					\
	../obj/str.obj 				\
	../obj/channel_socket.obj 	\
	../obj/sockets.obj			\
	../obj/ctypes.obj				\
	../obj/disk.obj				\
	../obj/atire_api_remote.obj	\
	../obj/file_internals.obj	



ant_cgi.exe : ant_cgi.c
	cl /MT -D_CRT_SECURE_NO_WARNINGS /W3 /Zi /Tp ant_cgi.c $(LIBS)

ant_getdoc_cgi.exe : ant_getdoc_cgi.c
	cl /MT -D_CRT_SECURE_NO_WARNINGS /W3 /Zi /Tp ant_getdoc_cgi.c $(LIBS)



clean :
	del *.obj *.exe *.ilk *.pdb
