OS_TYPE := $(shell uname)

###############################################################################
# The following options are the default compilation flags.
###############################################################################

# debugging or normal compiling and linking
USE_GCC := 1
USE_GCC_VERBOSE := 0
USE_GCC_DEBUG := 0

# use intel c/c++ compile
USE_INTEL_C := 0

# Prepare the binary for profiling
USE_PREPARE_PROFILING := 0

# profiling using gprof
USE_GPROF := 0

# if use print for debug
USE_PDEBUG := 0

# use the system memory management
USE_PURIFY := 0

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
USE_BUILT_IN_ZLIB := 1
USE_BUILT_IN_BZLIB := 1

# use LZO compression lib
# Please use either system or built-in libs, not both
USE_SYSTEM_LZO := 0
USE_BUILT_IN_LZO := 1

# use google's snappy compression library
USE_SNAPPY := 1

# if to add a term_max_impact to the structure
# of the dictionary for each term (This is just
# a temporary flag. In future, this should be
# added to the structure by default).
USE_TERM_LOCAL_MAX_IMPACT := 0

# print stats without converting millisecond to second, minute, hour
# for profiling purpose
USE_PRINT_TIME_NO_CONVERSION := 0

# construct impact headers for easy handling the quantums of the postings (version 0.4 of the index)
USE_IMPACT_HEADER := 1

# place the external document ID (its "filename") in the index and build and index on that for fast access (version 0.5 of the index)
USE_FILENAME_INDEX := 1

# print extra info for quantum-at-a-time and term-at-a-time
USE_PRINT_QUANTUM_STATS := 0

# partial decompression of postings list
USE_PARTIAL_DCOMPRESSION := 1

# represent the static array in two dimensions, reduce accumulators
# initialisation time
USE_TWO_D_ACCUMULATORS := 1
USE_TWO_D_ACCUMULATORS_POW2_WIDTH := 1

# what type to use for the accumulators
override CFLAGS += -DANT_ACCUMULATOR_T="double"
override CFLAGS += -DANT_PREGEN_T="unsigned long long"
override CFLAGS += -DDOUBLE_HASH

# use mysql database backend
USE_MYSQL := 0

# use Paice Husk Stemmer
USE_STEM_PAICE_HUSK := 1

# use snowball stemmers
USE_SNOWBALL := 1


###############################################################################
# specified your own setting in a separate file to override the default
#
# Just copy above config options to a new file with the name
# called "GNUmakeifle.specific.include"
#
# PLEASE do not commit this file to the repository.
#
# also, it is possible to include a file written for a specific target like iOS
# for example, make TARGET=iOS, GNUMakile.iOS will be included,
# however, a generic iOS make file so far is not successful
# you might want to use autotools the configurations files of which are
# also included in this project
#
###############################################################################
-include GNUmakefile.specific.include
-include GNUmakefile.${TARGET}


###############################################################################
# Please use above options to enable corresponding flags
###############################################################################

ifeq ($(USE_GCC), 1)
	CC = @g++ 
	PUT_FILENAME = @echo $<
	ifeq ($(USE_GCC_VERBOSE), 1)
		CC = g++ 
		#CC = /opt/local/bin/g++-mp-4.4
		PUT_FILENAME =
	endif
endif

ifeq ($(USE_INTEL_C), 1)
	CC = icpc
endif

BASE_DIR := $(shell pwd)

ZLIB_DIR := external/unencumbered/zlib
ZLIB_VERSION := zlib-1.2.5

BZIP_DIR := external/unencumbered/bzip
BZIP_VERSION := bzip2-1.0.6

SNAPPY_DIR := external/unencumbered/snappy
SNAPPY_VERSION := snappy-1.0.4

LZO_DIR := external/gpl/lzo
LZO_VERSION := lzo-2.06

SNOWBALL_DIR := external/unencumbered/snowball
SNOWBALL_VERSION := libstemmer_c

ifeq ($(USE_GCC_DEBUG), 1)
	LDFLAGS += -g -ggdb
	override CFLAGS += -g -ggdb -DDEBUG
	ifeq ($(OS_TYPE), Darwin)
		LDFLAGS += -gdwarf-2
		override CFLAGS += -gdwarf-2
	endif
else
	#LDFLAGS +=
	override CFLAGS += -O3 -fno-tree-vectorize
endif

ifeq ($(USE_PREPARE_PROFILING), 1)
	LDFLAGS += -g
	override CFLAGS += -g -O2
endif

ifeq ($(USE_GPROF), 1)
	LDFLAGS += -pg
	override CFLAGS += -pg
endif

ifeq ($(USE_PURIFY), 1)
	override CFLAGS += -DPURIFY
endif

ifeq ($(OS_TYPE), SUNOS)
	LDFLAGS += -lsocket -lnsl
endif

rb := -1
HASHER := HEADER_NUM

# common flags
LDFLAGS += -ldl
override CFLAGS += -x c++ -Wall -D${HASHER}=1 -DHASHER=1 -DONE_PARSER -D__STDC_LIMIT_MACROS -DREBALANCE_FACTOR=$(rb) \
					-Wno-missing-braces -Wno-unknown-pragmas -Wno-write-strings \
					-Wno-sign-compare -Wno-parentheses

ifeq ($(USE_FILENAME_INDEX), 1)
	override CFLAGS += -DFILENAME_INDEX
endif

ifeq ($(USE_IMPACT_HEADER), 1)
	override CFLAGS += -DIMPACT_HEADER
endif

ifeq ($(USE_PRINT_QUANTUM_STATS), 1)
	override CFLAGS += -DPRINT_QUANTUM_STATS
endif

ifeq ($(USE_SPECIAL_COMPRESSION), 1)
	override CFLAGS += -DSPECIAL_COMPRESSION
endif

# link the pthread library even if the parallel indexing is not enabled
ifeq ($(OS_TYPE), SUNOS)
	LDFLAGS += -lpthread
else
	LDFLAGS += -pthread
endif

ifeq ($(USE_PARALLEL_INDEXING), 1)
	override CFLAGS += -DPARALLEL_INDEXING -DPARALLEL_INDEXING_DOCUMENTS
endif

ifeq ($(USE_SYSTEM_ZLIB), 1)
	override CFLAGS += -DANT_HAS_ZLIB
	LDFLAGS += -lz
endif

ifeq ($(USE_SYSTEM_BZLIB), 1)
	# use system libbz2 library
	override CFLAGS += -DANT_HAS_BZLIB
	LDFLAGS += -lbz2
endif

ifeq ($(USE_BUILT_IN_ZLIB), 1)
	override CFLAGS += -DANT_HAS_ZLIB -I $(ZLIB_DIR)/$(ZLIB_VERSION)
	EXTRA_OBJS += $(ZLIB_DIR)/libz.a
endif

ifeq ($(USE_BUILT_IN_BZLIB), 1)
	override CFLAGS += -DANT_HAS_BZLIB -I $(BZIP_DIR)/$(BZIP_VERSION)
	EXTRA_OBJS += $(BZIP_DIR)/libbz2.a
endif

ifeq ($(USE_SYSTEM_LZO), 1)
	override CFLAGS += -DANT_HAS_LZO -I /usr/include/lzo
	LDFLAGS += -llzo2
endif

ifeq ($(USE_BUILT_IN_LZO), 1)
	override CFLAGS += -DANT_HAS_LZO -I $(LZO_DIR)/$(LZO_VERSION)/include/lzo -I $(LZO_DIR)/$(LZO_VERSION)/include
	EXTRA_OBJS += $(LZO_DIR)/liblzo2.a
endif

ifeq ($(USE_TERM_LOCAL_MAX_IMPACT), 1)
	override CFLAGS += -DTERM_LOCAL_MAX_IMPACT
endif

ifeq ($(USE_PDEBUG), 1)
	override CFLAGS += -DPDEBUG
endif

ifeq ($(USE_PRINT_TIME_NO_CONVERSION), 1)
	override CFLAGS += -DPRINT_TIME_NO_CONVERSION
endif

ifeq ($(USE_PARTIAL_DCOMPRESSION), 1)
	override CFLAGS += -DTOP_K_READ_AND_DECOMPRESSOR
endif

ifeq ($(USE_TWO_D_ACCUMULATORS), 1)
	override CFLAGS += -DTWO_D_ACCUMULATORS
endif

ifeq ($(USE_TWO_D_ACCUMULATORS_POW2_WIDTH), 1)
	ifeq ($(USE_TWO_D_ACCUMULATORS), 0)
		# The next line has to be spaces indented, not tabs. (something funny with GNU make)
        $(error TWO_D_ACCUMULATORS_POW2_WIDTH requries TWO_D_ACCUMULATORS to be enabled)
	endif
	override CFLAGS += -DTWO_D_ACCUMULATORS_POW2_WIDTH
endif

ifeq ($(USE_MYSQL), 1)
	override CFLAGS += -DANT_HAS_MYSQL $(shell mysql_config --cflags)
	LDFLAGS += $(shell mysql_config --libs)
endif

ifeq ($(USE_SNAPPY), 1)
	override CFLAGS += -DANT_HAS_SNAPPYLIB -I $(SNAPPY_DIR)/$(SNAPPY_VERSION)
	EXTRA_OBJS += $(SNAPPY_DIR)/libsnappy.a
endif

ifeq ($(USE_SNOWBALL), 1)
	override CFLAGS += -DANT_HAS_SNOWBALL -I $(SNOWBALL_DIR)/$(SNOWBALL_VERSION)/include
	EXTRA_OBJS += $(SNOWBALL_DIR)/libstemmer.a
endif

###############################################################################
# source files and compile commands
###############################################################################
ATIRE_DIR = atire
SRC_DIR = source
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib
TOOLS_DIR = tools
TESTS_DIR = tests

IGNORE_LIST := $(SRC_DIR)/stem_paice_husk.c

MAIN_FILES := $(ATIRE_DIR)/atire.c \
              $(ATIRE_DIR)/index.c \
              $(ATIRE_DIR)/atire_client.c \
              $(ATIRE_DIR)/atire_broker.c \
              $(ATIRE_DIR)/atire_dictionary.c \
              $(ATIRE_DIR)/atire_merge.c \
              $(ATIRE_DIR)/atire_doclist.c

ALL_SOURCES := $(shell ls $(ATIRE_DIR)/*.c $(SRC_DIR)/*.c)
SOURCES := $(filter-out $(MAIN_FILES) $(IGNORE_LIST), $(ALL_SOURCES))

ifeq ($(USE_STEM_PAICE_HUSK), 1)
	override CFLAGS += -DANT_HAS_PAICE_HUSK
	SOURCES += $(SRC_DIR)/stem_paice_husk.c
endif

SOURCES_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(notdir $(SOURCES))))


INDEX_SOURCES := index.c $(notdir $(SOURCES))
INDEX_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(INDEX_SOURCES)))

ATIRE_DICT_SOURCES := atire_dictionary.c $(notdir $(SOURCES))
ATIRE_DICT_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_DICT_SOURCES)))

ATIRE_CLIENT_SOURCES := atire_client.c $(notdir $(SOURCES))
ATIRE_CLIENT_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_CLIENT_SOURCES)))

ATIRE_SOURCES := atire.c $(notdir $(SOURCES))
ATIRE_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_SOURCES)))

ATIRE_MERGE_SOURCES := atire_merge.c $(notdir $(SOURCES))
ATIRE_MERGE_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_MERGE_SOURCES)))

ATIRE_BROKER_SOURCES := atire_broker.c $(notdir $(SOURCES))
ATIRE_BROKER_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_BROKER_SOURCES)))

ATIRE_DOCLIST_SOURCES := atire_doclist.c $(notdir $(SOURCES))
ATIRE_DOCLIST_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_DOCLIST_SOURCES)))

TOOLS_IGNORES := $(TOOLS_DIR)/mysql_xml_dump.c
TOOLS_SOURCES := $(notdir $(filter-out $(TOOLS_IGNORES), $(shell ls $(TOOLS_DIR)/*.c)))
TOOLS_EXES := $(basename $(TOOLS_SOURCES))
TOOLS_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(TOOLS_SOURCES)))

TESTS_IGNORES :=
TESTS_SOURCES := $(notdir $(filter-out $(TESTS_IGNORES), $(shell ls $(TESTS_DIR)/*.c)))
TESTS_EXES := $(basename $(TESTS_SOURCES))
TESTS_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(TESTS_SOURCES)))


all: $(EXTRA_OBJS) GNUmakefile index atire atire_client atire_broker atire_dictionary atire_merge atire_doclist

# faster compilation without considering extra objects, useful for repeated make for testing
internal: index atire atire_client atire_broker atire_dictionary atire_merge atire_doclist

index: $(BIN_DIR)/index
atire: $(BIN_DIR)/atire
atire_client: $(BIN_DIR)/atire_client
atire_broker: $(BIN_DIR)/atire_broker
atire_dictionary: $(BIN_DIR)/atire_dictionary
atire_merge: $(BIN_DIR)/atire_merge
atire_doclist: $(BIN_DIR)/atire_doclist

tools: $(TOOLS_EXES)

tests: $(TESTS_EXES)

info:
	@echo "OS_TYPE:" $(OS_TYPE)

test_all_source:
	@echo $(ALL_SOURCES)

test_source:
	@echo $(SOURCES)

test_index:
	@echo $(INDEX_OBJECTS)

test_ant:
	@echo $(ANT_OBJECTS)

test_atire:
	@echo $(ATIRE_OBJECTS)

test_tools:
	@echo $(TOOLS_SOURCES)
	@echo $(TOOLS_EXES)
	@echo $(TOOLS_OBJECTS)

test_tests:
	@echo $(TESTS_SOURCES)
	@echo $(TESTS_EXES)
	@echo $(TESTS_OBJECTS)

$(OBJ_DIR)/%.o : $(ATIRE_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(TOOLS_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(TESTS_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) -Isource -c $< -o $@

$(BIN_DIR)/index : $(INDEX_OBJECTS)
	$(CC) -o $@  $^ $(EXTRA_OBJS) $(LDFLAGS)

$(BIN_DIR)/atire_client : $(ATIRE_CLIENT_OBJECTS)
	$(CC) -o $@  $^ $(EXTRA_OBJS) $(LDFLAGS)

$(BIN_DIR)/atire : $(ATIRE_OBJECTS)
	$(CC) -o $@  $^ $(EXTRA_OBJS) $(LDFLAGS)

$(BIN_DIR)/atire_broker : $(ATIRE_BROKER_OBJECTS)
	$(CC) -o $@  $^ $(EXTRA_OBJS) $(LDFLAGS)

$(BIN_DIR)/atire_dictionary : $(ATIRE_DICT_OBJECTS)
	$(CC) -o $@  $^ $(EXTRA_OBJS) $(LDFLAGS)

$(BIN_DIR)/atire_merge : $(ATIRE_MERGE_OBJECTS)
	$(CC) -o $@  $^ $(EXTRA_OBJS) $(LDFLAGS)

$(BIN_DIR)/atire_doclist : $(ATIRE_DOCLIST_OBJECTS)
	$(CC) -o $@  $^ $(EXTRA_OBJS) $(LDFLAGS)

# Hacked to compile every single source in the tools directory.
# The $(TOOLS_OBJECTS) is requied at the dependencies so that
# all sources in tools can be compiled first. Then only the individual
# source is linked at a time.
$(TOOLS_EXES) : $(SOURCES_OBJECTS) $(TOOLS_OBJECTS)
	$(CC) -o $(BIN_DIR)/$@  $(OBJ_DIR)/$(notdir $@).o $(SOURCES_OBJECTS) $(EXTRA_OBJS) $(LDFLAGS)

$(TESTS_EXES) : $(SOURCES_OBJECTS) $(TESTS_OBJECTS)
	$(CC) -o $(BIN_DIR)/$@  $(OBJ_DIR)/$(notdir $@).o $(SOURCES_OBJECTS) $(EXTRA_OBJS) $(LDFLAGS)


$(SNAPPY_DIR)/libsnappy.a:
	@(cd ./$(SNAPPY_DIR); $(MAKE) -f GNUmakefile.static USE_GCC_VERBOSE=$(USE_GCC_VERBOSE); cd $(BASE_DIR);)

$(ZLIB_DIR)/libz.a:
	@(cd ./$(ZLIB_DIR); $(MAKE) -f GNUmakefile USE_GCC_VERBOSE=$(USE_GCC_VERBOSE); cd $(BASE_DIR);)

$(BZIP_DIR)/libbz2.a:
	@(cd ./$(BZIP_DIR); $(MAKE) -f GNUmakefile USE_GCC_VERBOSE="$(USE_GCC_VERBOSE)"; cd $(BASE_DIR);)

$(LZO_DIR)/liblzo2.a:
	@(cd ./$(LZO_DIR); $(MAKE) -f GNUmakefile USE_GCC_VERBOSE=$(USE_GCC_VERBOSE); cd $(BASE_DIR);)

$(SNOWBALL_DIR)/libstemmer.a:
	@(cd ./$(SNOWBALL_DIR); $(MAKE) -f GNUmakefile USE_GCC_VERBOSE=$(USE_GCC_VERBOSE); cd $(BASE_DIR);)

.PHONY : clean
clean :
	(cd ./$(SNAPPY_DIR); $(MAKE) -f GNUmakefile.static clean; cd $(BASE_DIR);)
	(cd ./$(ZLIB_DIR); $(MAKE) -f GNUmakefile clean; cd $(BASE_DIR);)
	(cd ./$(BZIP_DIR); $(MAKE) -f GNUmakefile clean; cd $(BASE_DIR);)
	(cd ./$(LZO_DIR); $(MAKE) -f GNUmakefile clean; cd $(BASE_DIR);)
	(cd ./$(SNOWBALL_DIR); $(MAKE) -f GNUmakefile clean; cd $(BASE_DIR);)
	\rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*

clean-internal:
	\rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*

depend :
	@makedepend  -f- -Y -o.o -w1024 -pobj/ source/*.c tools/*.c atire/*.c Link-The-Wiki/*.c | sed -e "s/obj\/source/obj/" | sed -e "s/obj\/tools/obj/" | sed -e "s/obj\/atire/obj/" | sed -e "s/obj\/Link-The-Wiki/obj/" > GNUmakefile.dependencies

include GNUmakefile.dependencies
