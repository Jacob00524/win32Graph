# Default architecture (can be overridden: `make ARCH=32`)
ARCH ?= 64

BUILD_FOLDER = build$(ARCH)
TARGET = wingraph
TARGET_DLL = $(BUILD_FOLDER)/$(TARGET).dll
TARGET_LIB = $(BUILD_FOLDER)/lib$(TARGET).a
TARGET_DEF = $(BUILD_FOLDER)/$(TARGET).def

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, $(BUILD_FOLDER)/%.o, $(SRC))

CC-32 = i686-w64-mingw32-gcc
CC-64 = x86_64-w64-mingw32-gcc

WIN-32=i686-w64-mingw32
WIN-64=x86_64-w64-mingw32

WININC-32 = /usr/$(WIN-32)/include
WININC-64 = /usr/$(WIN-64)/include

OTHER_INCLUDE = -Iinclude

CC = $(CC-$(ARCH))
WININC = $(WININC-$(ARCH))
WIN = $(WIN-$(ARCH))

all: $(TARGET_DLL) install test

$(BUILD_FOLDER)/%.o: src/%.c
	@mkdir -p $(BUILD_FOLDER)
	$(CC) -c $< -o $@ -I$(WININC) $(OTHER_INCLUDE) -Wall -Wextra

$(TARGET_DLL): $(OBJ)
	$(CC) -shared -mwindows -o $@ $^ \
		-Wl,--output-def,$(TARGET_DEF) \
		-Wl,--out-implib,$(TARGET_LIB)

install:
	rm -rf /usr/$(WIN)/include/$(TARGET)
	cp -r include /usr/$(WIN)/include/$(TARGET)
	cp -f $(TARGET_LIB) /usr/$(WIN)/lib/

test:
	cp $(TARGET_DLL) test/
	$(MAKE) -C test ARCH=$(ARCH)

clean:
	rm -rf $(BUILD_FOLDER)
	rm -rf /usr/$(WIN)/include/$(TARGET)
	rm -f /usr/$(WIN)/lib/lib$(TARGET).a
	$(MAKE) -C test clean ARCH=$(ARCH)

clean-all:
	make clean ARCH=64
	make clean ARCH=32

.PHONY: all clean test
