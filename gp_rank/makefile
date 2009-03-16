OBJDIR = ..\bin
BINDIR = ..\bin

CFLAGS = /W4 -D_CRT_SECURE_NO_WARNINGS /nologo /Zi /EHsc 
CC = @cl

.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

PARTS = $(OBJDIR)\gp_main.obj \
		$(OBJDIR)\gp.obj \
		$(OBJDIR)\gp_individual.obj \
		$(OBJDIR)\gp_individual_node.obj \

gp.exe : $(PARTS)
	$(CC) $(CFLAGS) $** /Fe$@

clean :
	del *.bak $(PARTS) gp.exe

