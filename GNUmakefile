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

#####
# This file has all the configuration options,
# for inclusion in other projects that might need them
include GNUmakefile.defns

all: $(EXTRA_OBJS) GNUmakefile index atire atire_client atire_broker atire_dictionary atire_merge atire_reorder atire_doclist

# faster compilation without considering extra objects, useful for repeated make for testing
internal: index atire atire_client atire_broker atire_dictionary atire_merge atire_reorder atire_doclist

index: $(BIN_DIR)/index
atire: $(BIN_DIR)/atire
atire_client: $(BIN_DIR)/atire_client
atire_broker: $(BIN_DIR)/atire_broker
atire_dictionary: $(BIN_DIR)/atire_dictionary
atire_merge: $(BIN_DIR)/atire_merge
atire_reorder: $(BIN_DIR)/atire_reorder
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
	$(CC) $(CFLAGS) $(MINUS_D) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) $(MINUS_D) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(TOOLS_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) $(MINUS_D) -Isource -c $< -o $@

$(OBJ_DIR)/%.o : $(TESTS_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) $(MINUS_D) -Isource -c $< -o $@

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

$(BIN_DIR)/atire_reorder : $(ATIRE_REORDER_OBJECTS)
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
	$(MAKE) -C $(SNAPPY_DIR) -f GNUmakefile.static USE_GCC_VERBOSE=$(USE_GCC_VERBOSE)

$(ZLIB_DIR)/libz.a:
	$(MAKE) -C $(ZLIB_DIR) -f GNUmakefile USE_GCC_VERBOSE=$(USE_GCC_VERBOSE)

$(BZIP_DIR)/libbz2.a:
	$(MAKE) -C $(BZIP_DIR) -f GNUmakefile USE_GCC_VERBOSE=$(USE_GCC_VERBOSE)

$(LZO_DIR)/liblzo2.a:
	$(MAKE) -C $(LZO_DIR) -f GNUmakefile USE_GCC_VERBOSE=$(USE_GCC_VERBOSE)

$(SNOWBALL_DIR)/libstemmer.a:
	$(MAKE) -C $(SNOWBALL_DIR) -f GNUmakefile USE_GCC_VERBOSE=$(USE_GCC_VERBOSE)

.PHONY : clean
clean :
	$(MAKE) -C $(SNAPPY_DIR) -f GNUmakefile.static clean
	$(MAKE) -C $(ZLIB_DIR) -f GNUmakefile clean
	$(MAKE) -C $(BZIP_DIR) -f GNUmakefile clean
	$(MAKE) -C $(LZO_DIR) -f GNUmakefile clean
	$(MAKE) -C $(SNOWBALL_DIR) -f GNUmakefile clean
	\rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*

clean-internal:
	\rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*

depend :
	@makedepend  -f- -Y -o.o -w1024 -pobj/ source/*.c tools/*.c atire/*.c Link-The-Wiki/*.c | sed -e "s/obj\/source/obj/" | sed -e "s/obj\/tools/obj/" | sed -e "s/obj\/atire/obj/" | sed -e "s/obj\/Link-The-Wiki/obj/" > GNUmakefile.dependencies

include GNUmakefile.dependencies
