BUILD_ROOT=../..
include $(BUILD_ROOT)/common_defs.mk

LIB = libled.a

all: $(LIB)
	@echo "Done"

SRC_DIR = ./src

SRCS = $(wildcard  $(addsuffix /*.c, $(SRC_DIR)))
OBJS = $(patsubst %.c, %.o, $(SRCS))
DEPS = $(patsubst %.c, %.d, $(SRCS))

$(LIB): $(OBJS)
	@echo "Linking library $@"
	$(MAKE_LIBRARY)

clean:
	@echo "Cleaning directory"
	@rm -f $(OBJS) $(DEPS) $(LIB) 
