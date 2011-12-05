all : getsyns.exe wordnet_test.exe

getsyns.exe : getsyns.c
	cl /Zi /TP getsyns.c \ant\source\disk.c \ant\source\file.c \ant\source\file_internals.c /link /fixed:no

wordnet_test.exe : wordnet_test.c wordnet.c
	cl /Zi /TP wordnet_test.c wordnet.c \ant\source\disk.c \ant\source\file.c \ant\source\file_internals.c /link /fixed:no

clean:
	del *.obj *.exe *.ilk *.bak *.pdb *.sln

