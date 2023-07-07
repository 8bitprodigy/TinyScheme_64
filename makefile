BUILD_DIR=build
include $(N64_INST)/include/n64.mk

CFLAGS = -Wno-unused-function

src = TinyScheme_64.c scheme.c usb.c

all: TinyScheme_64.z64

$(BUILD_DIR)/TinyScheme_64.elf: $(src:%.c=$(BUILD_DIR)/%.o)

TinyScheme_64.z64: N64_ROM_TITLE="TinyScheme 64"

clean:
	rm -rf $(BUILD_DIR) TinyScheme_64.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
