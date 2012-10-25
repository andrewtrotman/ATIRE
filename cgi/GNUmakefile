CC := g++
#CFLAGS  := -O3
CFLAGS  := -g  -I ../source

SRCDIR = ../source
OBJDIR = ../obj
BINDIR = ../bin
APIDIR = ../atire

COMMON_SOURCES := 				\
	../source/str.c 			\
	../source/channel_socket.c 		\
	../source/sockets.c			\
	../source/disk.c			\
	../source/ctypes.c			\
	../source/file_internals.c		\
	../source/unicode.c \
	../source/unicode_tables.c \
	../atire/atire_api_remote.c
	
COMMON_NOTDIR_SOURCES := $(notdir $(COMMON_SOURCES))
COMMON_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(COMMON_NOTDIR_SOURCES)))

all: ant_cgi.exe ant_getdoc_cgi.exe

ant_cgi.exe: $(COMMON_OBJECTS) ant_cgi.o
	$(CC) $(LDFLAGS) -o $@ $^
	
ant_getdoc_cgi.exe: $(COMMON_OBJECTS) ant_getdoc_cgi.o
	$(CC) $(LDFLAGS) -o $@ $^
	
#ant_cgi.o: ant_cgi.c $(COMMON_SOURCES)

#ant_getdoc_cgi.o: ant_getdoc_cgi.c $(COMMON_SOURCES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
    	
$(OBJDIR)/atire_api_remote.o: $(APIDIR)/atire_api_remote.c
	$(CC) $(CFLAGS) -c $< -o $@
	    	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean :
	\rm *.o ant_cgi.exe ant_getdoc_cgi.exe
	
test_common_objects:
	@echo $(COMMON_OBJECTS)
	
test_common_sources:
	@echo $(COMMON_SOURCES)
