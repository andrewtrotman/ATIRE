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

# construct impact headers for easy handling the quantums of the postings
USE_IMPACT_HEADER := 0

# partial decompression of postings list
USE_PARTIAL_DCOMPRESSION := 1

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
USE_STEM_LOVINS := 1

# use Paice Husk Stemmer
USE_STEM_PAICE_HUSK := 1


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

ifeq ($(USE_IMPACT_HEADER), 1)
	CFLAGS += -DIMPACT_HEADER
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

ifeq ($(USE_PARTIAL_DCOMPRESSION), 1)
	CFLAGS += -DTOP_K_READ_AND_DECOMPRESSOR
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
ATIRE_DIR = atire
SRC_DIR = source
OBJ_DIR = obj
BIN_DIR = bin
PHP_DIR = php_ext
LIB_DIR = lib
TOOLS_DIR = tools

IGNORE_LIST := $(SRC_DIR)/ant_ant.c \
			  $(SRC_DIR)/ant_plugins.c \
			  $(SRC_DIR)/test_compression.c \
			  $(SRC_DIR)/stem_lovins.c \
			  $(SRC_DIR)/stem_paice_husk.c

MAIN_FILES := $(ATIRE_DIR)/atire.c \
			  $(ATIRE_DIR)/index.c \
			  $(ATIRE_DIR)/atire_client.c \
			  $(ATIRE_DIR)/atire_broker.c \
			  $(TOOLS_DIR)/ant_dictionary.c

ALL_SOURCES := $(shell ls $(ATIRE_DIR)/*.c $(SRC_DIR)/*.c)
SOURCES := $(filter-out $(IGNORE_LIST) $(MAIN_FILES), $(ALL_SOURCES))

ifeq ($(USE_STEM_LOVINS), 1)
	CFLAGS += -DANT_HAS_LOVINS
	SOURCES += $(SRCDIR)/stem_lovins.c
endif

ifeq ($(USE_STEM_PAICE_HUSK), 1)
	CFLAGS += -DANT_HAS_PAICE_HUSK
	SOURCES += $(SRCDIR)/stem_paice_husk.c
endif

INDEX_SOURCES := index.c $(notdir $(SOURCES))
INDEX_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(INDEX_SOURCES)))

ANT_DICT_SOURCES := ant_dictionary.c $(notdir $(SOURCES))
ANT_DICT_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ANT_DICT_SOURCES)))

ATIRE_CLIENT_SOURCES := atire_client.c $(notdir $(SOURCES))
ATIRE_CLIENT_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_CLIENT_SOURCES)))

ATIRE_SOURCES := atire.c $(notdir $(SOURCES))
ATIRE_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_SOURCES)))

ATIRE_BROKER_SOURCES := atire_broker.c $(notdir $(SOURCES))
ATIRE_BROKER_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(ATIRE_BROKER_SOURCES)))

PHP_EXT_SOURCES := $(notdir $(shell ls $(PHP_DIR)/*.c))
PHP_EXT_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(PHP_EXT_SOURCES)))

PREGEN_PREC_SOURCES := pregen_precision_measurement.c $(notdir $(SOURCES))
PREGEN_PREC_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(PREGEN_PREC_SOURCES)))

MYSQL_XML_DUMP_SOURCES := mysql_xml_dump.c $(notdir $(SOURCES))
MYSQL_XML_DUMP_OBJECTS := $(addprefix $(OBJ_DIR)/, $(subst .c,.o, $(MYSQL_XML_DUMP_SOURCES)))

all : info $(EXTRA_OBJS) index atire atire_client atire_broker ant_dictionary

index : $(BIN_DIR)/index
atire: $(BIN_DIR)/atire
atire_client: $(BIN_DIR)/atire_client
atire_broker: $(BIN_DIR)/atire_broker
ant_dictionary: $(BIN_DIR)/ant_dictionary

php_ext : $(LIB_DIR)/atire.so

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

$(OBJ_DIR)/%.o : $(ATIRE_DIR)/%.c
	$(CC) $(CFLAGS) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(TOOLS_DIR)/%.c
	$(CC) $(CFLAGS) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(PHP_DIR)/%.c
	$(CC) $(PHP_CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o : tools/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR)/atire.so : $(PHP_EXT_OBJECTS) $(ATIRE_CLIENT_OBJECTS)
	$(CC) $(PHP_LDFLAGS) -o $@ $^

$(BIN_DIR)/index : $(INDEX_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BIN_DIR)/atire_client : $(ATIRE_CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BIN_DIR)/atire : $(ATIRE_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BIN_DIR)/atire_broker : $(ATIRE_BROKER_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BIN_DIR)/ant_dictionary : $(ANT_DICT_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BIN_DIR)/pregen_precision_measurement : $(PREGEN_PREC_OBJECTS)
	$(CC) $(LDFLAGS) -o $@  $^ $(EXTRA_OBJS)

$(BIN_DIR)/mysql_xml_dump : $(MYSQL_XML_DUMP_OBJECTS)
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
	\rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*

depend :
	makedepend  -f- -Y -o.obj -w1024 -pbin/ source/*.c tools/*.c atire/*.c Link-The-Wiki/*.c | sed -e "s/bin\/source/bin/" | sed -e "s/bin\/tools/bin/" | sed -e "s/bin\/atire/bin/" | sed -e "s/bin\/Link-The-Wiki/bin/" > GNUmakefile.dependencies

include GNUmakefile.dependencies
