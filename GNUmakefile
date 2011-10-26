OS_TYPE := $(shell uname)
CC = g++
#CC = icpc

###############################################################################
# The following options are the default compilation flags.
###############################################################################

# debugging or normal compiling and linking
USE_GCC_DEBUG := 0

# Prepare the binary for profiling
USE_PREPARE_PROFILING := 0

# profiling using gprof
USE_GPROF := 0

# if use print for debug
USE_PDEBUG := 0

# special compression for index file
# better enabled by defualt
USE_SPECIAL_COMPRESSION := 1

# parallel compiling and linking
USE_PARALLEL_INDEXING := 1

# Please use either system or built-in libs, not both
#
# use system libz and libbz2 libraries
USE_SYSTEM_ZLIB := 0
USE_SYSTEM_BZLIB := 0
# use ant's built-in libz and libbz2 libraries
USE_BUILT_IN_ZLIB := 0
USE_BUILT_IN_BZLIB := 0

# use LZO compression lib
# Please use either system or built-in libs, not both
USE_SYSTEM_LZO := 0
USE_BUILT_IN_LZO := 0

# if to add a term_max_impact to the structure
# of the dictionary for each term (This is just
# a temporary flag. In future, this should be
# added to the structure by default).
USE_TERM_LOCAL_MAX_IMPACT := 0

# print stats without converting millisecond to second, minute, hour
# for profiling purpose
USE_PRINT_TIME_NO_CONVERSION := 0

# when the whole index file is read into memory, then there is no need
# to memcpy for read.
USE_DIRECT_MEMORY_READ := 0

# enable either TOP_K search or HEAP_K search, but not both
USE_TOP_K_SEARCH := 0
USE_HEAP_K_SEARCH := 1

# if top_k or heap_k is enable, then partial decompression of postings list
# is supported
USE_PARTIAL_DCOMPRESSION := 0

# represent the static array in two dimensions, reduce accumulators
# initialisation time
USE_TWO_D_ACCUMULATORS := 1

# what type to use for the accumulators
CFLAGS += -DANT_ACCUMULATOR_T="unsigned short"
CFLAGS += -DANT_PREGEN_T="unsigned long long"

# use mysql database backend
USE_MYSQL := 0

# build a php extension for Atire
USE_PHP_EXTENSION := 0

# use google's snappy compression library
USE_SNAPPY := 0

###############################################################################
# specified your own setting in a separate file to override the default
#
# Just copy above config options to a new file with the name
# called "GNUmakeifle.specific.include"
#
# PLEASE do not commit this file to the repository.
###############################################################################
-include GNUmakefile.specific.include


###############################################################################
# Please use above options to enable corresponding flags
###############################################################################
ifeq ($(USE_GCC_DEBUG), 1)
	LDFLAGS += -g -ggdb
	CFLAGS += -g -ggdb -DDEBUG
	ifeq ($(OS_TYPE), Darwin)
		LDFLAGS += -gdwarf-2
		CFLAGS += -gdwarf-2
	endif
else
	#LDFLAGS +=
	CFLAGS += -O3
endif

ifeq ($(USE_PREPARE_PROFILING), 1)
	LDFLAGS += -g
	CFLAGS += -g -O2
endif

ifeq ($(USE_GPROF), 1)
	LDFLAGS += -pg
	CFLAGS += -pg
endif

ifeq ($(OS_TYPE), SUNOS)
	LDFLAGS += -lsocket -lnsl
endif

# common flags
LDFLAGS += -ldl
CFLAGS += -Wall -DHASHER=1 -DHEADER_HASHER=1 -DONE_PARSER -D__STDC_LIMIT_MACROS \
					-Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings \
					-Wno-sign-compare -Wno-parentheses

ifeq ($(USE_SPECIAL_COMPRESSION), 1)
	CFLAGS += -DSPECIAL_COMPRESSION
endif

ifeq ($(USE_PARALLEL_INDEXING), 1)
   ifeq ($(OS_TYPE), SUNOS)
	LDFLAGS += -lpthread
   else
	LDFLAGS += -pthread
   endif
	CFLAGS += -DPARALLEL_INDEXING -DPARALLEL_INDEXING_DOCUMENTS
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
	EXTRA_OBJS += zlib/libz.a
endif

ifeq ($(USE_BUILT_IN_BZLIB), 1)
	CFLAGS += -DANT_HAS_BZLIB -I bzip/bzip2-1.0.5
	EXTRA_OBJS += bzip/libbz2.a
endif

ifeq ($(USE_SYSTEM_LZO), 1)
	CFLAGS += -DANT_HAS_LZO -I /usr/include/lzo
	LDFLAGS += -llzo2
endif

ifeq ($(USE_BUILT_IN_LZO), 1)
	CFLAGS += -DANT_HAS_LZO -I lzo/lzo-2.05/include/lzo lzo/lzo-2.05/include
	EXTRA_OBJS += lzo/liblzo2.a
endif

ifeq ($(USE_TERM_LOCAL_MAX_IMPACT), 1)
	CFLAGS += -DTERM_LOCAL_MAX_IMPACT
endif

ifeq ($(USE_PDEBUG), 1)
	CFLAGS += -DPDEBUG
endif

ifeq ($(USE_PRINT_TIME_NO_CONVERSION), 1)
	CFLAGS += -DPRINT_TIME_NO_CONVERSION
endif

ifeq ($(USE_DIRECT_MEMORY_READ), 1)
	CFLAGS += -DDIRECT_MEMORY_READ
endif

ENABLED_TOP_K_SEARCH := no

ifeq ($(USE_TOP_K_SEARCH), 1)
	CFLAGS += -DTOP_K_SEARCH
	ENABLED_TOP_K_SEARCH := yes
else ifeq ($(USE_HEAP_K_SEARCH), 1)
	CFLAGS += -DHEAP_K_SEARCH
	ENABLED_TOP_K_SEARCH := yes
endif


# make sure either top_k or heap_k is enabled
ifeq ($(ENABLED_TOP_K_SEARCH), yes)
	ifeq ($(USE_PARTIAL_DCOMPRESSION), 1)
		CFLAGS += -DTOP_K_READ_AND_DECOMPRESSOR
	endif
endif

ifeq ($(USE_TWO_D_ACCUMULATORS), 1)
	CFLAGS += -DTWO_D_ACCUMULATORS
endif

ifeq ($(USE_MYSQL), 1)
	CFLAGS += -DANT_HAS_MYSQL $(shell mysql_config --cflags)
	LDFLAGS += $(shell mysql_config --libs)
endif

ifeq ($(USE_PHP_EXTENSION), 1)
        PHP_CFLAGS = -fPIC -DPIC
        PHP_CFLAGS += $(shell php-config --includes)
        PHP_LDFLAGS = -shared -export-dynamic -avoid-version -prefer-pic -module
endif

ifeq ($(USE_SNAPPY), 1)
	CFLAGS += -DANT_HAS_SNAPPYLIB -I snappy/snappy-1.0.4
	EXTRA_OBJS += snappy/libsnappy.a
endif

###############################################################################
# source files and compile commands
###############################################################################
SRCDIR = source
OBJDIR = obj
BINDIR = bin
PHPDIR = php_ext
LIBDIR = lib

IGNORE_LIST := $(SRCDIR)/ant_ant.c \
			  $(SRCDIR)/ant_api.c \
			  $(SRCDIR)/ant_plugins.c \
			  $(SRCDIR)/test_compression.c

MAIN_FILES := $(SRCDIR)/ant.c \
			  $(SRCDIR)/atire.c \
			  $(SRCDIR)/index.c \
			  $(SRCDIR)/ant_dictionary.c \
			  $(SRCDIR)/atire_client.c \
			  $(SRCDIR)/atire_broker.c \

ALL_SOURCES := $(shell ls $(SRCDIR)/*.c)
SOURCES := $(filter-out $(IGNORE_LIST) $(MAIN_FILES), $(ALL_SOURCES))

INDEX_SOURCES := index.c $(notdir $(SOURCES))
INDEX_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(INDEX_SOURCES)))

ANT_SOURCES := ant.c $(notdir $(SOURCES))
ANT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(ANT_SOURCES)))

ANT_DICT_SOURCES := ant_dictionary.c $(notdir $(SOURCES))
ANT_DICT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(ANT_DICT_SOURCES)))

ATIRE_CLIENT_SOURCES := atire_client.c $(notdir $(SOURCES))
ATIRE_CLIENT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(ATIRE_CLIENT_SOURCES)))

ATIRE_SOURCES := atire.c $(notdir $(SOURCES))
ATIRE_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(ATIRE_SOURCES)))

ATIRE_BROKER_SOURCES := atire_broker.c $(notdir $(SOURCES))
ATIRE_BROKER_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(ATIRE_BROKER_SOURCES)))

PHP_EXT_SOURCES := $(notdir $(shell ls $(PHPDIR)/*.c))
PHP_EXT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(PHP_EXT_SOURCES)))

PREGEN_PREC_SOURCES := pregen_precision_measurement.c $(notdir $(SOURCES))
PREGEN_PREC_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(PREGEN_PREC_SOURCES)))

MYSQL_XML_DUMP_SOURCES := mysql_xml_dump.c $(notdir $(SOURCES))
MYSQL_XML_DUMP_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(MYSQL_XML_DUMP_SOURCES)))

all : info $(EXTRA_OBJS) $(BINDIR)/index $(BINDIR)/ant $(BINDIR)/atire $(BINDIR)/atire_client $(BINDIR)/atire_broker $(BINDIR)/ant_dictionary

php_ext : $(LIBDIR)/atire.so

info:
	@echo "OS_TYPE:" $(OS_TYPE)

test_source:
	@echo $(SOURCES)

test_index:
	@echo $(INDEX_OBJECTS)

test_ant:
	@echo $(ANT_OBJECTS)

test_atire:
	@echo $(ATIRE_OBJECTS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : $(PHPDIR)/%.c
	$(CC) $(PHP_CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : tools/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBDIR)/atire.so : $(PHP_EXT_OBJECTS) $(ATIRE_CLIENT_OBJECTS)
	$(CC) $(PHP_LDFLAGS) -o $@ $^

$(BINDIR)/index : $(INDEX_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BINDIR)/atire_client : $(ATIRE_CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BINDIR)/ant : $(ANT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BINDIR)/atire : $(ATIRE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BINDIR)/atire_broker : $(ATIRE_BROKER_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BINDIR)/ant_dictionary : $(ANT_DICT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)
	
$(BINDIR)/pregen_precision_measurement : $(PREGEN_PREC_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BINDIR)/mysql_xml_dump : $(MYSQL_XML_DUMP_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

snappy/libsnappy.a:
	(cd ./snappy; $(MAKE) -f GNUmakefile.static CC=$(CC); cd ..;)

zlib/libz.a:
	(cd ./zlib; $(MAKE) -f GNUmakefile; cd ..;)

bzip/libbz2.a:
	(cd ./bzip; $(MAKE) -f GNUmakefile; cd ..;)

lzo/liblzo2.a:
	(cd ./lzo; $(MAKE) -f GNUmakefile; cd ..;)

.PHONY : clean
clean :
	(cd ./snappy; $(MAKE) -f GNUmakefile.static clean; cd ..;)
	(cd ./zlib; $(MAKE) -f GNUmakefile clean; cd ..;)
	(cd ./bzip; $(MAKE) -f GNUmakefile clean; cd ..;)
	(cd ./lzo; $(MAKE) -f GNUmakefile clean; cd ..;)
	\rm -f $(OBJDIR)/*.o $(BINDIR)/*

depend :
	makedepend -f- -Y -w1024 -pbin/ source/*.c -- $(CFLAGS) | sed -e "s/bin\/source/bin/" >| GNUmakefile.dependencies

include GNUmakefile.dependencies
