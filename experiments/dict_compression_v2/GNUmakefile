OS_TYPE := $(shell uname)
CC = g++
#CC := /opt/local/bin/g++-mp-4.4
#CC = icpc

###############################################################################
# The following options are the default compilation flags.
###############################################################################

# debugging or normal compiling and linking
USE_GCC_DEBUG := 1

# Prepare the binary for profiling
USE_PREPARE_PROFILING := 0

# profiling using gprof
USE_GPROF := 0

# if use print for debug
USE_PDEBUG := 1

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

# use Lovins stemmer
USE_STEM_LOVINS := 0

# use Paice Husk Stemmer
USE_STEM_PAICE_HUSK := 0


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

COMMON_PREFIX := -DCOMMON_PREFIX_SIZE=4

ifeq ($(OS_TYPE), SUNOS)
	LDFLAGS += -lsocket -lnsl
endif

# common flags
LDFLAGS += -ldl
CFLAGS += -Wall -DHASHER=1 -DHEADER_HASHER=1 $(COMMON_PREFIX) -DONE_PARSER -D__STDC_LIMIT_MACROS \
   	     -Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings \
   	     -Wno-sign-compare -Wno-parentheses

# flags which are system dependent
ifeq ($(OS_TYPE), Darwin)
	#
else
	CFLAGS += -Wno-unused-result
endif

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

###############################################################################
# source files and compile commands
###############################################################################
SRCDIR = ../../source
OBJDIR = bin
BINDIR = bin

IGNORE_LIST := $(SRCDIR)/ant_ant.c \
			  $(SRCDIR)/ant_api.c \
			  $(SRCDIR)/ant_plugins.c \
			  $(SRCDIR)/ant_dictionary.c \
			  $(SRCDIR)/ant.c \
			  $(SRCDIR)/index.c \
			  $(SRCDIR)/test_compression.c \
			  $(SRCDIR)/atire_client.c \
			  $(SRCDIR)/atire_broker.c \
			  $(SRCDIR)/atire.c

ALL_SOURCES := $(shell ls $(SRCDIR)/*.c)
SOURCES := $(filter-out $(IGNORE_LIST), $(ALL_SOURCES))

ANT_SOURCES := $(notdir $(SOURCES))
ANT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(ANT_SOURCES)))

DICT_SOURCES := common.c dict_fixed.c dict_string.c dict_blocked.c dict_embed.c dict_embedfront.c dict_embedfixed.c dict_embedfixed_null.c
BUILD_VOCAB_SOURCES := build_vocab.c $(DICT_SOURCES)
BUILD_VOCAB_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(BUILD_VOCAB_SOURCES)))
BUILD_DICT_SOURCES := build_dictionaries.c $(DICT_SOURCES)
BUILD_DICT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(BUILD_DICT_SOURCES)))
SEARCH_DICT_SOURCES := search_dictionaries.c $(DICT_SOURCES)
SEARCH_DICT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(SEARCH_DICT_SOURCES)))

ANT_INDEX_SOURCES := $(SOURCES) $(SRCDIR)/index.c
ANT_INDEX_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(notdir $(ANT_INDEX_SOURCES))))

ANT_ANT_SOURCES := $(SOURCES) $(SRCDIR)/ant.c
ANT_ANT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(notdir $(ANT_ANT_SOURCES))))

ANT_DICT_SOURCES := $(SOURCES) $(SRCDIR)/ant_dictionary.c
ANT_DICT_OBJECTS := $(addprefix $(OBJDIR)/, $(subst .c,.o, $(notdir $(ANT_DICT_SOURCES))))

all : build_vocabs build_dict search_dict ant_index ant_ant ant_dict

build_vocabs: $(OBJDIR)/build_vocab
build_dict: $(OBJDIR)/build_dictionaries
search_dict: $(OBJDIR)/search_dictionaries
ant_index: $(OBJDIR)/ant_index
ant_ant: $(OBJDIR)/ant_ant
ant_dict: $(OBJDIR)/ant_dict

test_source:
	@echo $(SOURCES)

test_build_dict:
	@echo $(BUILD_DICT_OBJECTS)

test_search_dict:
	@echo $(SEARCH_DICT_OBJECTS)

test_ant_index:
	@echo $(ANT_INDEX_OBJECTS)

test_os:
	@echo $(OS_TYPE)

#compile ANT source
$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#compile local source
$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(SRCDIR)

$(OBJDIR)/build_vocab : $(ANT_OBJECTS) $(BUILD_VOCAB_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(EXTRA_OBJS) $^

$(OBJDIR)/build_dictionaries : $(ANT_OBJECTS) $(BUILD_DICT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(EXTRA_OBJS) $^

$(OBJDIR)/search_dictionaries : $(ANT_OBJECTS) $(SEARCH_DICT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(EXTRA_OBJS) $^

$(OBJDIR)/ant_index : $(ANT_INDEX_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(EXTRA_OBJS) $^

$(OBJDIR)/ant_ant : $(ANT_ANT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(EXTRA_OBJS) $^

$(OBJDIR)/ant_dict : $(ANT_INDEX_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(EXTRA_OBJS) $^

.PHONY : clean
clean :
	\rm -f $(OBJDIR)/*.o $(BINDIR)/*

depend :
	makedepend -f- -Y -w1024 -pbin/ $(SRCDIR)/*.c *.c -- $(CFLAGS) | sed -e "s/bin\/source/bin/" >| GNUmakefile.dependencies

include GNUmakefile.dependencies
