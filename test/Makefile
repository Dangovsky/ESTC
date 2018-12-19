BUILD_ROOT=../..
OBJS = main.o

BINARY_NAME = main

LDLIBS += -lled
LIB_DIRS  += $(BUILD_ROOT)/projects/libled/
INCLUDE_DIRS += -I$(BUILD_ROOT)/projects/libled/include

include $(BUILD_ROOT)/common_defs.mk

.PHONY: $(BINARY_NAME)

$(BINARY_NAME): $(BINARY_NAME).bin

clean:
	@rm -f $(OBJS) main.elf main.bin main.elf.map main.d

flash: $(BINARY_NAME)
	$(FLASH_BINARY)
