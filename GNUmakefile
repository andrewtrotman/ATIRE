CC = g++
#CC = icpc

###############################################################################
# The following options are used to enable corresponding compilation flags.
###############################################################################

# debugging or normal compiling and linking
USE_GCC_DEBUG = 0

# if use print for debug
USE_PDEBUG = 0

# special compression for index file
USE_SPECIAL_COMPRESSION = 0

# parallel compiling and linking
USE_PARALLEL_INDEXING = 1

# Please use either system or built-in libs, not both
#
# use system libz and libbz2 libraries
USE_SYSTEM_ZLIB = 0
USE_SYSTEM_BZLIB = 0
# use ant's built-in libz and libbz2 libraries
USE_BUILT_IN_ZLIB = 0
USE_BUILT_IN_BZLIB = 0

# if to add a term_max_impact to the structure
# of the dictionary for each term (This is just
# a temporary flag. In future, this should be
# added to the structure by default).
USE_TERM_LOCAL_MAX_IMPACT = 0


###############################################################################
# Please use above options to enable corresponding flags
###############################################################################
ifeq ($(USE_GCC_DEBUG), 1)
	LDFLAGS = -g -ldl
	CFLAGS = -g -Wall -DHASHER=1 -DHEADER_HASHER=1 -DONE_PARSER -DTOP_K_SEARCH \
    	     -Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings \
    	     -Wno-sign-compare -Wno-parentheses
else
	LDFLAGS = -ldl
	CFLAGS = -Wall -DHASHER=1 -DHEADER_HASHER=1 -O3 -DONE_PARSER -DTOP_K_SEARCH \
	         -Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings \
	         -Wno-sign-compare -Wno-parentheses
endif

ifeq ($(USE_SPECIAL_COMPRESSION), 1)
	CFLAGS += -DSPECIAL_COMPRESSION
endif

ifeq ($(USE_PARALLEL_INDEXING), 1)
	LDFLAGS += -pthread
	CFLAGS += -DPARALLEL_INDEXING
endif

ifeq ($(USE_SYSTEM_ZLIB), 1)
	CFLAGS += -DANT_HAS_ZLIB
	LDFLAGS += -lz
endif

ifeq ($(USE_SYSTEM_BZLIB), 1)
	# use system libbz2 library
	CFLAGS += -DANT_HAS_BZLIB
	LDFLAGS += -lbz2
endif

ifeq ($(USE_BUILT_IN_ZLIB), 1)
	CFLAGS += -DANT_HAS_ZLIB -I zlib/zlib-1.2.3
	EXTRA_OBJS += zlib/zlib-1.2.3/libz.a
endif

ifeq ($(USE_BUILT_IN_BZLIB), 1)
	CFLAGS += -DANT_HAS_BZLIB -I bzip/bzip2-1.0.5
	EXTRA_OBJS += bzip/bzip2-1.0.5/libbz2.a
endif

ifeq ($(USE_TERM_LOCAL_MAX_IMPACT), 1)
	CFLAGS += -DTERM_LOCAL_MAX_IMPACT
endif

ifeq ($(USE_PDEBUG), 1)
	CFLAGS += -DPDEBUG
endif


###############################################################################
# source files and compile commands
###############################################################################
SRCDIR = source
OBJDIR = bin
BINDIR = bin

IGNORE_LIST = $(SRCDIR)/ant_ant.c \
			  $(SRCDIR)/ant_api.c \
			  $(SRCDIR)/ant_plugins.c \
			  $(SRCDIR)/ant_dictionary.c \
			  $(SRCDIR)/test_compression.c

ALL_SOURCES = $(shell ls $(SRCDIR)/*.c)
SOURCES = $(filter-out $(IGNORE_LIST), $(ALL_SOURCES))

INDEX_SOURCES = $(filter-out ant.c, $(notdir $(SOURCES)))
INDEX_OBJECTS = $(addprefix $(OBJDIR)/, $(subst .c,.o, $(INDEX_SOURCES)))

ANT_SOURCES = $(filter-out index.c, $(notdir $(SOURCES)))
ANT_OBJECTS = $(addprefix $(OBJDIR)/, $(subst .c,.o, $(ANT_SOURCES)))


all : $(BINDIR)/index $(BINDIR)/ant

test_source:
	@echo $(SOURCES)

test_index:
	@echo $(INDEX_OBJECTS)

test_ant:
	@echo $(ANT_OBJECTS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/index : $(INDEX_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(EXTRA_OBJS) $^

$(BINDIR)/ant : $(ANT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(EXTRA_OBJS) $^

.PHONY : clean
clean :
	\rm -f $(OBJDIR)/*.o $(BINDIR)/index $(BINDIR)/ant

depend :
	makedepend -f- -Y -w1024 -pbin/ source/*.c -- $(CFLAGS) | sed -e "s/bin\/source/bin/" >| makefile.dependencies.unix

include GNUmakefile.dependencies
