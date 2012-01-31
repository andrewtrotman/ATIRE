
ifeq ($(USE_GCC_VERBOSE),1)
	CC = gcc
	PUT_FILENAME =
else
	CC = @gcc
	PUT_FILENAME = @echo $<
endif

ZLIB = libz.a
SOURCE_DIR = zlib-1.2.5

ALL_SOURCES = $(SOURCE_DIR)/adler32.c \
			  $(SOURCE_DIR)/compress.c \
			  $(SOURCE_DIR)/crc32.c \
			  $(SOURCE_DIR)/deflate.c \
			  $(SOURCE_DIR)/infback.c \
			  $(SOURCE_DIR)/inffast.c \
			  $(SOURCE_DIR)/inflate.c \
			  $(SOURCE_DIR)/inftrees.c \
			  $(SOURCE_DIR)/trees.c \
			  $(SOURCE_DIR)/uncompr.c \
			  $(SOURCE_DIR)/zutil.c

ALL_OBJECTS := $(subst .c,.o, $(ALL_SOURCES))

all: $(ZLIB)

test:
	@echo $(ALL_SOURCES)

$(ZLIB): $(ALL_OBJECTS)
	@ar rvs $(ZLIB) $(ALL_OBJECTS)

$(SOURCE_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(ZLIB) $(ALL_OBJECTS)
