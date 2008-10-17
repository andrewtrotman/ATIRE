SRCDIR = source
OBJDIR = bin
BINDIR = bin

PARTS = \
	$(OBJDIR)\disk.obj \
	$(OBJDIR)\disk_internals.obj \
	$(OBJDIR)\main.obj


{$(SRCDIR)\}.c{$(OBJDIR)\}.obj:
	$(CC) $(CFLAGS) /c /Tp $< /Fo$@

all : $(BINDIR)\main.exe

$(BINDIR)\main.exe : $(PARTS)
	$(CC) $(CFLAGS) $(PARTS) /Fe$@

$(OBJDIR)\disk_internals.obj : $(SRCDIR)\disk_internals.c $(SRCDIR)\disk_internals.h

$(OBJDIR)\disk.obj : $(SRCDIR)\disk.c  $(SRCDIR)\disk.h $(SRCDIR)\disk_internals.h

clean :
	del $(OBJDIR)\*.obj $(BINDIR)\*.exe
