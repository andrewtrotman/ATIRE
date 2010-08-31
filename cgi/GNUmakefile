CC := g++
CFLAGS  := -O3

SRCDIR = ../source
OBJDIR = ../bin
BINDIR = ../bin

COMMON_SOURCES := 				\
	../source/str.c 			\
	../source/channel_socket.c 	\
	../source/sockets.c			\
	../source/disk.c			\
	../source/file_internals.c	
	
COMMON_NOTDIR_SOURCES := $(notdir $(COMMON_SOURCES))
COMMON_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(COMMON_NOTDIR_SOURCES)))

all: ant_cgi ant_getdoc_cgi

ant_cgi: $(COMMON_OBJECTS) ant_cgi.o
	$(CC) $(LDFLAGS) -o $@ $^
	
ant_getdoc_cgi: $(COMMON_OBJECTS) ant_getdoc_cgi.o
	$(CC) $(LDFLAGS) -o $@ $^
	
#ant_cgi.o: ant_cgi.c $(COMMON_SOURCES)

#ant_getdoc_cgi.o: ant_getdoc_cgi.c $(COMMON_SOURCES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
    	
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean :
	\rm *.o ant_cgi ant_getdoc_cgi
	
test_common_objects:
	@echo $(COMMON_OBJECTS)
	
test_common_sources:
	@echo $(COMMON_SOURCES)