
CC = @gcc

LIBSNOWBALL = libstemmer.a
SOURCE_DIR = libstemmer_c

ALL_SOURCES := $(shell ls $(SOURCE_DIR)/src_c/*.c $(SOURCE_DIR)/runtime/*.c)

ALL_OBJECTS := $(subst .c,.o, $(ALL_SOURCES))

CFLAGS += -D_FILE_OFFSET_BITS=64

all: $(LIBSNOWBALL)

test:
	@echo $(ALL_SOURCES)

$(LIBSNOWBALL): $(ALL_OBJECTS)
	ar rvs $(LIBSNOWBALL) $(ALL_OBJECTS)

$(SOURCE_DIR)/src_c/%.o: $(SOURCE_DIR)/src_c/%.c
	@echo $<
	$(CC) $(CFLAGS) -c $< -o $@

$(SOURCE_DIR)/runtime/%.o: $(SOURCE_DIR)/runtime/%.c
	@echo $<
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIBSNOWBALL) $(ALL_OBJECTS)
