
ifeq ($(USE_GCC_VERBOSE), 1)
	CC = gcc
	PUT_FILENAME =
else
	CC = @gcc
	PUT_FILENAME = @echo $<
endif

SOURCE_DIR = lzo-2.06/src
LZO = liblzo2.a

ALL_SOURCES =  $(SOURCE_DIR)/lzo1.c		\
	$(SOURCE_DIR)/lzo1_99.c		\
	$(SOURCE_DIR)/lzo1a.c		\
	$(SOURCE_DIR)/lzo1a_99.c	\
	$(SOURCE_DIR)/lzo1b_1.c		\
	$(SOURCE_DIR)/lzo1b_2.c		\
	$(SOURCE_DIR)/lzo1b_3.c		\
	$(SOURCE_DIR)/lzo1b_4.c		\
	$(SOURCE_DIR)/lzo1b_5.c		\
	$(SOURCE_DIR)/lzo1b_6.c		\
	$(SOURCE_DIR)/lzo1b_7.c		\
	$(SOURCE_DIR)/lzo1b_8.c		\
	$(SOURCE_DIR)/lzo1b_9.c		\
	$(SOURCE_DIR)/lzo1b_99.c	\
	$(SOURCE_DIR)/lzo1b_9x.c	\
	$(SOURCE_DIR)/lzo1b_cc.c	\
	$(SOURCE_DIR)/lzo1b_d1.c	\
	$(SOURCE_DIR)/lzo1b_d2.c	\
	$(SOURCE_DIR)/lzo1b_rr.c	\
	$(SOURCE_DIR)/lzo1b_xx.c	\
	$(SOURCE_DIR)/lzo1c_1.c		\
	$(SOURCE_DIR)/lzo1c_2.c		\
	$(SOURCE_DIR)/lzo1c_3.c		\
	$(SOURCE_DIR)/lzo1c_4.c		\
	$(SOURCE_DIR)/lzo1c_5.c		\
	$(SOURCE_DIR)/lzo1c_6.c		\
	$(SOURCE_DIR)/lzo1c_7.c		\
	$(SOURCE_DIR)/lzo1c_8.c		\
	$(SOURCE_DIR)/lzo1c_9.c		\
	$(SOURCE_DIR)/lzo1c_99.c	\
	$(SOURCE_DIR)/lzo1c_9x.c	\
	$(SOURCE_DIR)/lzo1c_cc.c	\
	$(SOURCE_DIR)/lzo1c_d1.c	\
	$(SOURCE_DIR)/lzo1c_d2.c	\
	$(SOURCE_DIR)/lzo1c_rr.c	\
	$(SOURCE_DIR)/lzo1c_xx.c	\
	$(SOURCE_DIR)/lzo1f_1.c		\
	$(SOURCE_DIR)/lzo1f_9x.c	\
	$(SOURCE_DIR)/lzo1f_d1.c	\
	$(SOURCE_DIR)/lzo1f_d2.c	\
	$(SOURCE_DIR)/lzo1x_1.c		\
	$(SOURCE_DIR)/lzo1x_1k.c	\
	$(SOURCE_DIR)/lzo1x_1l.c	\
	$(SOURCE_DIR)/lzo1x_1o.c	\
	$(SOURCE_DIR)/lzo1x_9x.c	\
	$(SOURCE_DIR)/lzo1x_d1.c	\
	$(SOURCE_DIR)/lzo1x_d2.c	\
	$(SOURCE_DIR)/lzo1x_d3.c	\
	$(SOURCE_DIR)/lzo1x_o.c		\
	$(SOURCE_DIR)/lzo1y_1.c		\
	$(SOURCE_DIR)/lzo1y_9x.c	\
	$(SOURCE_DIR)/lzo1y_d1.c	\
	$(SOURCE_DIR)/lzo1y_d2.c	\
	$(SOURCE_DIR)/lzo1y_d3.c	\
	$(SOURCE_DIR)/lzo1y_o.c		\
	$(SOURCE_DIR)/lzo1z_9x.c	\
	$(SOURCE_DIR)/lzo1z_d1.c	\
	$(SOURCE_DIR)/lzo1z_d2.c	\
	$(SOURCE_DIR)/lzo1z_d3.c	\
	$(SOURCE_DIR)/lzo2a_9x.c	\
	$(SOURCE_DIR)/lzo2a_d1.c	\
	$(SOURCE_DIR)/lzo2a_d2.c	\
	$(SOURCE_DIR)/lzo_crc.c		\
	$(SOURCE_DIR)/lzo_init.c	\
	$(SOURCE_DIR)/lzo_ptr.c		\
	$(SOURCE_DIR)/lzo_str.c		\
	$(SOURCE_DIR)/lzo_util.c


ALL_OBJECTS := $(subst .c,.o, $(ALL_SOURCES))

all: $(LZO)

test:
	@echo $(ALL_SOURCES)

$(LZO): $(ALL_OBJECTS)
	@ar rvs $(LZO) $(ALL_OBJECTS)

$(SOURCE_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(PUT_FILENAME)
	$(CC) $(CFLAGS) -c $< -o $@ -I $(SOURCE_DIR)/../include

clean:
	rm -f $(LZO) $(ALL_OBJECTS)
