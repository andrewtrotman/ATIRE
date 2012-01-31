
ifeq ($(USE_GCC_VERBOSE), 1)
	CC = gcc
	PUT_FILENAME =
else
	CC = @gcc
	PUT_FILENAME = @echo $<
endif

BZLIB = libbz2.a
SOURCE_DIR = bzip2-1.0.6

ALL_SOURCES = $(SOURCE_DIR)/blocksort.c \
			  $(SOURCE_DIR)/huffman.c \
			  $(SOURCE_DIR)/crctable.c \
			  $(SOURCE_DIR)/randtable.c \
			  $(SOURCE_DIR)/compress.c \
			  $(SOURCE_DIR)/decompress.c \
			  $(SOURCE_DIR)/bzlib.c \

ALL_OBJECTS := $(subst .c,.o, $(ALL_SOURCES))

all: $(BZLIB)

test:
	@echo $(ALL_SOURCES)

$(BZLIB): $(ALL_OBJECTS)
	@ar rvs $(BZLIB) $(ALL_OBJECTS)

$(SOURCE_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BZLIB) $(ALL_OBJECTS)
